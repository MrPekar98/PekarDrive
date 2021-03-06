#include <stdio.h>
#include <stdlib.h>
#include <comm.h>
#include <PP/packet.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include "file_exec.h"
#include "boot.h"
#include "argument.h"
#include "../util/logger.h"

#define READ_SIZE 10000

void *handle_client(void *arg);
void answer_client(conn client, unsigned seq_number, enum type msg_type, struct file_output output);
static inline struct packet create_packet(unsigned seq_number, enum type msg_type, const char *arg);
const char *file_list_to_str(struct file_list fl);

int main(int argc, char **argv)
{
    char *ip = parse_to_ip(argv, argc);

    if (ip == NULL)
    {
        logger(ERROR, COMP_WORKER, get_parse_error());
        return 1;
    }

    else if (!boot(ip))
        return 1;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (!server_fd)
    {
#if defined(VERBOSE_1) || defined(VERBOSE_2)
        logger(ERROR, COMP_WORKER, "Connection setup failed.");
#endif
        return 1;
    }

    while (1)
    {
        pthread_t thread;
        conn client = conn_listen(server_fd, get_port());

        if (client.error)
        {
#ifdef VERBOSE_2
            char *msg = malloc(100);
            sprintf(msg, "Client connection refused: %s", client.error_msg);
            logger(WARNING, COMP_WORKER, msg);
            free(msg);
#endif
            conn_close(&client);
            close(server_fd);
            server_fd = socket(AF_INET, SOCK_STREAM, 0);
            continue;
        }

#ifdef THREADING
        pthread_create(&thread, NULL, handle_client, &client);
#else
        handle_client(&client);
#endif
    }

    return 0;
}

// Client handler.
void *handle_client(void *arg)
{
    conn client = *((conn *) arg);
    void *buffer = malloc(READ_SIZE);
    ssize_t bytes = conn_read(client, buffer, READ_SIZE);

    if (bytes <= 0)
    {
#if defined(VERBOSE_1) || defined(VERBOSE_2)
        logger(ERROR, COMP_WORKER, "Error reading client.");
#endif
        free(buffer);
        conn_close(&client);
#ifdef THREADING
        pthread_exit(NULL);
#endif
        return NULL;
    }

    struct packet p = p_decode(buffer);     // TODO: Add error field to packet to let master know.

    if (p.token != MASTER_TKN)
    {
#if defined(VERBOSE_1) || defined(VERBOSE_2)
        logger(WARNING, COMP_WORKER, "Message not from client.");
#endif
        free(buffer);
        p_cleanup(p);
        conn_close(&client);
#ifdef THREADING
        pthread_exit(NULL);
#endif
        return NULL;
    }

    else if (p.msg_type == PING)
    {
        conn_write(client, p_encode(p_init(p.seq_number + 1, "1", PING)), 13);
#ifdef VERBOSE_2
        logger(MESSAGE, COMP_WORKER, "PING!");
#endif
    }

    else if (p.msg_type == LS)
    {
        const char *ls_output = file_list_to_str(file_list(WORKER_DIR));
        conn_write(client, p_encode(p_init(p.seq_number + 1, ls_output, LS)), strlen(ls_output) + sizeof(struct packet));
    }

    else
    {
        short op;

        switch (p.msg_type)
        {
            case READ:
                op = FILE_READ;
                break;

            case WRITE:
                op = FILE_WRITE;
                break;

            case APPEND:
                op = FILE_APPEND;
                break;

            case DELETE:
                op = FILE_DELETE;
                break;

            default:
                answer_client(client, p.seq_number + 1, -1, (struct file_output) {.error = 1});
        }

        struct file_output output = f_exec(op, p.arg);
        answer_client(client, p.seq_number + 1, p.msg_type, output);
    }

    p_cleanup(p);
    conn_close(&client);
    free(buffer);
#ifdef THREADING
    pthread_exit(NULL);
#endif

    return NULL;
}

// Answers client depending on file output.
void answer_client(conn client, unsigned seq_number, enum type msg_type, struct file_output output)
{
    struct packet p;

    if (output.error)
    {
        if (msg_type == WRITE)
            p = p_error(seq_number, "Write error", msg_type, WORKER_TKN);

        else if (msg_type == READ)
            p = p_error(seq_number, "Read error", msg_type, WORKER_TKN);

        else if (msg_type == APPEND)
            p = p_error(seq_number, "Append error", msg_type, WORKER_TKN);

        else
            p = p_error(seq_number, "Error", msg_type, WORKER_TKN);
    }

    else
        p = create_packet(seq_number, msg_type, (char *) output.out);

    conn_write(client, p_encode(p), sizeof(struct packet) + output.len + 1);
}

// Creates packet instance.
static inline struct packet create_packet(unsigned seq_number, enum type msg_type, const char *arg)
{
    struct packet p = p_init(seq_number, arg, msg_type);
    attach_token(WORKER_TKN, &p);
    return p;
}

// Converts array of string into single string when new-line separator.
const char *file_list_to_str(struct file_list fl)
{
    if (fl.count == 0)
        return "";

    char *files = malloc(1);
    unsigned i;

    for (i = 0; i < fl.count; i++)
    {
        files = realloc(files, strlen(files) + 1 + strlen(fl.file_names[i]));
        strcat(files, fl.file_names[i]);

        if (i == fl.count - 1)
            strcat(files, "\0");

        else
            strcat(files, "\n");
    }

    return files;
}
