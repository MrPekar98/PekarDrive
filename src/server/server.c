#include <stdio.h>
#include "../fs/fs.h"
#include <comm.h>
#include "../server_table.h"
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <packet.h>

#define READ_SIZE 500

void *handle_client(void *arg);

int main()
{
    // TODO: Implement locking so that no two clients access the same file at once. No shared locks.
    // TODO: Add to /lib/ a packet format file that specifies message format. This must be included in interface.h.

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
            continue;
        }

        pthread_create(&thread, NULL, handle_client, &client);
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
        pthread_exit(NULL);
        return NULL;
    }

    struct packet p = p_decode(buffer);

    if (p.msg_type == PING)
        conn_write(client, p_encode(p_init(0, "1", PING)), 13);

    else
    {
        struct file_output output = f_exec(p.msg_type == READ ? FILE_READ | FILE_WRITE, p.arg, p.msg_len);
        conn_write(client, p_encode(p_init(p.seq_number + 1, (char *) output.out, p.msg_type)), output.len);
    }

    conn_close(&client);
    free(buffer);
    pthread_exit(NULL);
    return NULL;
}
