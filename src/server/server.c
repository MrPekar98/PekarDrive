#include <stdio.h>
#include "../fs/fs.h"
#include <comm.h>
#include "../server_table.h"
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <packet.h>
#include <unistd.h>
#include <string.h>
#include "file_exec.h"

#define READ_SIZE 500
#define MASTER_ADDR "127.0.0.1"
#define MASTER_PORT 45654

void register_worker();
const char *machine_ip();
void *handle_client(void *arg);
void answer_client(conn client, unsigned seq_number, enum type msg_type, struct file_output output);
const char *file_list_to_str(struct file_list fl);

int main()
{
    register_worker();
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (!server_fd)
    {
#ifdef LOG
        printf("Connection setup failed.\n");
#endif
        return 1;
    }

    while (1)
    {
        pthread_t thread;
        conn client = conn_listen(server_fd);

        if (client.error)
        {
#ifdef LOG
            printf("Client connection refused: %s\n", client.error_msg);
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

// Registers worker at master.
void register_worker()
{
    conn worker = client_init(MASTER_ADDR, MASTER_PORT);
    unsigned bytes = 0;
    char *msg = malloc(21);
    sprintf(msg, "%s;%d", machine_ip(), get_port());

    while ((bytes = conn_write(worker, p_encode(p_init(0, msg, REGISTER)), 40)) <= 0);
    free(msg);
    conn_close(&worker);
}

// Returns IP address of this machine.
const char *machine_ip()
{
    return "127.0.0.1";
}

// Client handler.
void *handle_client(void *arg)
{
    conn client = *((conn *) arg);
    void *buffer = malloc(READ_SIZE);
    ssize_t bytes = conn_read(client, buffer, READ_SIZE);

    if (bytes <= 0)
    {
#ifdef LOG
        printf("Error reading client.\n");
#endif
        free(buffer);
        conn_close(&client);
#ifdef THREADING
        pthread_exit(NULL);
#endif
        return NULL;
    }

    struct packet p = p_decode(buffer);     // TODO: Add error field to packet to let master know.

    if (p.msg_type == PING)
        conn_write(client, p_encode(p_init(p.seq_number + 1, "1", PING)), 13);

    else if (p.msg_type == LS)
    {
        const char *ls_output = file_list_to_str(file_list());
        conn_write(client, p_encode(p_init(p.seq_number + 1, ls_output, LS)), strlen(ls_output));
    }

    else if (p.msg_type == APPEND)
    {
        struct file_output output = f_exec(FILE_APPEND, p.arg);
        answer_client(client, p.seq_number + 1, p.msg_type, output);
    }

    else
    {
        struct file_output output = f_exec(p.msg_type == READ ? FILE_READ : FILE_WRITE, p.arg);
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
    if (output.error)
    {
        if (msg_type == WRITE)
            conn_write(client, p_encode(p_error(seq_number, "Write error", msg_type)), sizeof(struct packet) + 12);

        else if (msg_type == READ)
            conn_write(client, p_encode(p_error(seq_number, "Read error", msg_type)), sizeof(struct packet) + 11);

        else if (msg_type == APPEND)
            conn_write(client, p_encode(p_error(seq_number, "Append error", msg_type)), sizeof(struct packet) + 13);

        else
            conn_write(client, p_encode(p_error(seq_number, "Error", msg_type)), sizeof(struct packet) + 6);
    }

    else
        conn_write(client, p_encode(p_init(seq_number, (char *) output.out, msg_type)), output.len);
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
