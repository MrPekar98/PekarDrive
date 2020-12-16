#include <stdio.h>
#include "fs/fs.h"
#include <comm.h>
#include "server_table.h"
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>

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
            printf("Client connection refused.\n");
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

    pthread_exit(NULL);
    return NULL;
}
