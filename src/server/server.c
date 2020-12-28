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

void *handle_client(void *arg);
static const char *file_list_to_str(struct file_list fl);

int main()
{
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
        conn_write(client, p_encode(p_init(p.seq_number + 1, (char *) output.out, p.msg_type)), output.len);
    }

    else
    {
        struct file_output output = f_exec(p.msg_type == READ ? FILE_READ : FILE_WRITE, p.arg);
        conn_write(client, p_encode(p_init(p.seq_number + 1, (char *) output.out, p.msg_type)), output.len);
    }

    p_cleanup(p);
    conn_close(&client);
    free(buffer);
#ifdef THREADING
    pthread_exit(NULL);
#endif

    return NULL;
}

// Converts array of string into single string when new-line separator.
static const char *file_list_to_str(struct file_list fl)
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
