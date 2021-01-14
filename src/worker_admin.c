#include "worker_admin.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <comm.h>
#include <packet.h>

#define WORKER_READ_LEN 5000
#define WORKER_WRITE_LEN 100

#define PACKET(arg, type) p_encode(p_init(0, arg, type))

static unsigned update_time = 60;   // Seconds between pinging workers.

static void *manager_thread(void *arg);
static void ping_server(struct file_server *server);
static void handle_ping_response(conn worker);

// Sers update time for pinging workers.
void set_admin_update_time(unsigned time)
{
    update_time = time;
}

// Returns update time.
unsigned get_admin_update_time()
{
    return update_time;
}

// Adds worker to server table.
void add_worker(struct file_server worker)
{
    add_server(worker);
}

// Entrypoint for admin to look out for workers.
// Starts a new thread.
pthread_t start_admin()
{
    pthread_t thread;
    pthread_create(&thread, NULL, manager_thread, NULL);

    return thread;
}

// Thread for managing workers.
static void *manager_thread(void *arg)
{
    time_t start = time(NULL);

    while (1)
    {
        while (time(NULL) - start < update_time);

        start = time(NULL);
        for_each_server(ping_server);
    }
}

// Pings a file server,
static void ping_server(struct file_server *server)
{
#ifdef LOG
    printf("Ping to %d.\n", server->id);
#endif

    conn worker = client_init(server->location.ip, server->location.port);
    struct packet p = p_init(0, "", PING);
    attach_token(MASTER_TKN, &p);
    conn_write(worker, p_encode(p), sizeof(struct packet) + 10);
    handle_ping_response(worker);
}

// TODO: Handle failure.
// Handles ping response.
static void handle_ping_response(conn worker)
{
    void *buffer = malloc(sizeof(struct packet) + 10);
    int bytes = conn_read(worker, buffer, sizeof(struct packet) + 10);
    free(buffer);

    if (bytes <= 0)
    {

#ifdef LOG
        printf("Worker (file descriptor: %d) has failed.\n", worker.fd);
#endif
    }

#ifdef LOG
    else
        printf("Successful response from worker (file descriptor: %d).\n", worker.fd);
#endif
}

// Returns count of worker nodes.
unsigned short worker_count()
{
    return table_count();
}

// Executes ls on worker node found by index.
char *worker_ls(unsigned index)
{
    struct file_server *worker = getfs_index(index);
    conn client = client_init(worker->location.ip, worker->location.port);
    void *buffer = malloc(WORKER_READ_LEN);

    if (buffer == NULL)
        return NULL;

    else if (conn_write(client, PACKET("", LS), WORKER_WRITE_LEN) <= 0)
    {
        free(buffer);
        return NULL;
    }

    else if (conn_read(client, buffer, WORKER_READ_LEN) <= 0)
    {
        free(buffer);
        return NULL;
    }

    struct packet p = p_decode(buffer);
    free(buffer);
    return p.arg;
}
