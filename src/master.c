#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "worker_admin.h"
#include <comm.h>
#include <packet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define READ_SZ 10000
#define WRITE_SZ 10000

// Prototypes.
void server_restart(conn *client, int *fd);
void handle_client(conn client);
const char *handle_request(enum type msg_type, const char *arg);
short service_unavailable(conn client);
unsigned char_at(const char *str, char c);
const char *exec_str(enum type msg_type, const char *arg);
const char *exec_long(enum type msg_type, const char *arg);
const char *long2str(long val);

// Stores information about workers.
// Load balances workload based in worker information in server table.
// When client wants to read a file, find the file amongst the workers. This is also true when appending to a file.
int main()
{
    pthread_t admin = start_admin();
    int master_fd = socket(AF_INET, SOCK_STREAM, 0);

    while (1)
    {
        conn client = conn_listen(master_fd);

        if (client.error)
        {
#ifdef LOG
            printf("Master error: %s\n", client.error_msg);
#endif

            server_restart(&client, &master_fd);
            continue;
        }

        handle_client(client);
        server_restart(&client, &master_fd);
    }

    return 0;
}

// Closes connection and restarts server.
void server_restart(conn *client, int *fd)
{
    conn_close(client);
    close(*fd);
    *fd = socket(AF_INET, SOCK_STREAM, 0);
}

// Handler for incoming messages by client.
void handle_client(conn client)
{
    static unsigned worker_id = 1;
    void *buffer = malloc(500);

    if (buffer == NULL || conn_read(client, buffer, READ_SZ) <= 0)
    {
        free(buffer);
        return;
    }

    struct packet p = p_decode(buffer);

    if (p.msg_type != REGISTER && !service_unavailable(client))
    {
        free(buffer);
        return;
    }

    else if (p.msg_type == REGISTER)
    {
        if (p.token != WORKER_TKN)
        {
#ifdef LOG
            printf("Non-worker attempted to register.\n");
#endif
            free(buffer);
            return;
        }

        struct file_server worker = {.id = worker_id++, .location.ip = malloc(16), .location.port = get_port()};
        strncpy(worker.location.ip, p.arg, char_at(p.arg, ';') - 1);
        add_worker(worker);

#ifdef LOG
        printf("Registered new worker (%d):\n%s:%d\n\n", worker.id, worker.location.ip, worker.location.port);
#endif
    }

    else
    {
        const char *ret_val = handle_request(p.msg_type, p.arg);
        conn_write(client, ret_val, strlen(ret_val) + 1);
    }

    free(buffer);
}

// Checks service for being available.
// Tells client if service is unavailable.
short service_unavailable(conn client)
{
    if (!worker_count())
    {
        conn_write(client, "Service unavailable.", 21);
        return 0;
    }

    return 1;
}

// Handles message type.
const char *handle_request(enum type msg_type, const char *arg)
{
    switch (msg_type)
    {
        case LS: case READ:
            return exec_str(msg_type, arg);

        case WRITE: case APPEND:
            return exec_long(msg_type, arg);

        case DELETE:
            worker_delete(arg);
            return "1";

        default:
            return "Server error.";
    }
}

// For operators that return string.
const char *exec_str(enum type msg_type, const char *arg)
{
    switch (msg_type)
    {
        case LS:
            return worker_ls();

        case READ:
            return (char *) worker_read(arg);

        default:
            return "Server error.";
    }
}

// For operators that return long.
const char *exec_long(enum type msg_type, const char *arg)
{
    char *file = malloc(strlen(arg));
    char *data = malloc(strlen(arg));
    strncpy(file, arg, char_at(arg, ';') - 1);
    strcpy(data, arg + strlen(file) + 1);

    switch (msg_type)
    {
        case WRITE:
            return long2str(worker_write(file, data));

        case APPEND:
            return long2str(worker_append(file, data));

        default:
            return "Server error.";
    }
}

// Returns index at first occurence of char in string.
unsigned char_at(const char *str, char c)
{
    unsigned index = 0;
    while (str[index++] != c);
    return index;
}

// Converts long to string.
const char *long2str(long val)
{
    char *strval = malloc(10);
    sprintf(strval, "%ld, val");
    return strval;
}
