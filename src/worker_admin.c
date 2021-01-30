#include "worker_admin.h"
#include "balance.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <comm.h>
#include <packet.h>
#include <string.h>

#define WORKER_READ_LEN 5000
#define WORKER_WRITE_LEN 100

#define PACKET(arg, type) p_encode(p_init(0, arg, type))

static unsigned update_time = 60;   // Seconds between pinging workers.

static void *manager_thread(void *arg);
static void ping_server(struct file_server *server);
static void handle_ping_response(conn worker);
static struct file_server *find_by_file(const char *file);
static char *worker_ls_index(unsigned index);

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
    printf("Ping to %d (%s:%d).\n", server->id, server->location.ip, server->location.port);
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

// TODO: Test if working when reading, deleting and appending.
// Finds worker with given file name.
static struct file_server *find_by_file(const char *file)
{
    unsigned i, workers = worker_count();

    for (i = 0; i < workers; i++)
    {
        char *ls = worker_ls_index(i);

        if (strstr(ls, file) != NULL)
            return getfs_index(i);
    }

    return NULL;
}

// Executes LS given worker index.
static char *worker_ls_index(unsigned index)
{
    struct file_server *worker = getfs_index(index);
    conn client = client_init(worker->location.ip, worker->location.port);
    void *buffer = malloc(WORKER_READ_LEN);
    struct packet p = p_init(0, "", LS);
    attach_token(MASTER_TKN, &p);

    if (buffer == NULL)
        return NULL;

    else if (conn_write(client, p_encode(p), WORKER_WRITE_LEN) <= 0)
    {
        free(buffer);
        return NULL;
    }

    else if (conn_read(client, buffer, WORKER_READ_LEN) <= 0)
    {
        free(buffer);
        return NULL;
    }

    struct packet p_response = p_decode(buffer);
    free(buffer);
    return p_response.arg;
}

// Executes ls on worker node found by index.
char *worker_ls()
{
    unsigned i, workers = worker_count();
    char *result = malloc(1);

    for (i = 0; i < workers; i++)
    {
        char *ls = worker_ls_index(i);
        result = realloc(result, strlen(ls) + 1);
        sprintf(result, "%s%s%s", result, i != 0 ? "\n" : "", ls);
    }

    sprintf(result, "%s\0", result);
    return result;
}

// Writes new file to worker that has least load.
long worker_write(const char *file, const void *data)
{
    struct file_server *fs = least_load();

    if (fs == NULL)
    {
#ifdef LOG
        printf("No workers registrered.\n");
#endif
        return -1;
    }

    char *buffer = malloc(strlen(file) + strlen((char *) data) + 10);
    sprintf(buffer, "%s;%s\0", file, (char *) data);
    struct packet p = p_init(0, buffer, WRITE);
    attach_token(MASTER_TKN, &p);

    conn worker = client_init(fs->location.ip, fs->location.port);
    long bytes = conn_write(worker, p_encode(p), WORKER_WRITE_LEN + strlen((char *) data));
    conn_close(&worker);
    free(buffer);

    return bytes;
}

// TODO: finish this.
// Appends data to file at first worker found that has given file name.
long worker_append(const char *file, const void *data)
{
    return -1;
}

// TODO: Finish this.
// Reads all data from file.
void *worker_read(const char *file)
{
    return NULL;
}

// TODO: Finish this.
// Deletes file from first found worker with that file.
void worker_delete(const char *file)
{

}
