#include "worker_admin.h"
#include "balance.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <comm.h>
#include <PP/packet.h>
#include <string.h>
#include <signal.h>
#include "util/logger.h"

#define WORKER_READ_LEN 5000
#define WORKER_WRITE_LEN 100

#define PACKET(arg, type) p_encode(p_init(0, arg, type))

static unsigned update_time = 60;   // Seconds between pinging workers.

static void *manager_thread(void *arg);
static void ping_server(struct file_server *server);
static void handle_ping_response(conn worker, struct file_server worker_server);
static struct file_server *find_by_file(const char *file);
static char *worker_ls_index(unsigned index);
static long written_bytes(conn worker);


// Sets update time for pinging workers.
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

// Stops admin thread.
void end_admin(pthread_t pid)
{
    int ret = pthread_kill(pid, -9);    // -9 is sigkill.

#if defined(VERBOSE_1) || defined(VERBOSE_2)
    if (ret != 0)
        logger(ERROR, COMP_MASTER, "Could not kill admin thread.");
#endif
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
#if defined(VERBOSE_1) || defined(VERBOSE_2)
    char *msg = malloc(50);
    sprintf(msg, "Ping to %d (%s:%d).", server->id, server->location.ip, server->location.port);
    logger(MESSAGE, COMP_MASTER, msg);
    free(msg);
#endif

    conn worker = client_init(server->location.ip, server->location.port);
    struct packet p = p_init(0, "", PING);
    attach_token(MASTER_TKN, &p);
    conn_write(worker, p_encode(p), sizeof(struct packet) + 10);
    handle_ping_response(worker, *server);
}

// Handles ping response.
// Worker is deleted from local table if worker is not responding properly.
static void handle_ping_response(conn worker, struct file_server worker_server)
{
    void *buffer = malloc(sizeof(struct packet) + 10);
    int bytes = conn_read(worker, buffer, sizeof(struct packet) + 10);

    if (bytes <= 0)
    {
        remove_server(worker_server.id);
#if defined(VERBOSE_1) || defined(VERBOSE_2)
        char *msg = malloc(50);
        sprintf(msg, "Worker (file descriptor: %d) has failed.", worker.fd);
        logger(WARNING, COMP_MASTER, msg);
        free(msg);
#endif
    }

    struct packet p = p_decode(buffer);

    if (p.msg_type != PING)
    {
        remove_server(worker_server.id);
#if defined(VERBOSE_1) || defined(VERBOSE_2)
        char *msg = malloc(100);
        sprintf(msg, "Received response from worker (file descriptor: %d), but not from ping.", worker.fd);
        logger(MESSAGE, COMP_MASTER, msg);
        free(msg);
#endif
    }

#if defined(VERBOSE_1) || defined(VERBOSE_2)
    else
    {
        char *msg = malloc(50);
        sprintf(msg, "Successful response from worker (file descriptor: %d).", worker.fd);
        logger(MESSAGE, COMP_MASTER, msg);
        free(msg);
    }
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
    void *buffer = malloc(WORKER_READ_LEN + sizeof(struct packet));
    struct packet p = p_init(0, "", LS);
    attach_token(MASTER_TKN, &p);

    if (buffer == NULL)
        return NULL;

    else if (conn_write(client, p_encode(p), sizeof(struct packet) + 1) <= 0)
    {
        free(buffer);
        return NULL;
    }

    else if (conn_read(client, buffer, WORKER_READ_LEN + sizeof(struct packet)) <= 0)
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

        if (ls == NULL)
            continue;

        result = realloc(result, strlen(result) + strlen(ls) + 2);
        sprintf(result, "%s\n%s", result, ls);
    }

    result = realloc(result, strlen(result) + 1);
    sprintf(result, "%s\0", result);
    return result;
}

// Writes new file to worker that has least load.
long worker_write(const char *file, const void *data)
{
    struct file_server *fs = least_load();

    if (fs == NULL)
    {
#if defined(VERBOSE_1) || defined(VERBOSE_2)
        logger(ERROR, COMP_MASTER, "No workers registrered.");
#endif
        return -1;
    }

    char *buffer = malloc(strlen(file) + strlen((char *) data) + 2);
    sprintf(buffer, "%s;%s\0", file, (char *) data);
    struct packet p = p_init(0, buffer, WRITE);
    attach_token(MASTER_TKN, &p);

    conn worker = client_init(fs->location.ip, fs->location.port);
    conn_write(worker, p_encode(p), strlen(buffer) + sizeof(struct packet) + 1);
    free(buffer);

    long bytes = written_bytes(worker);
    conn_close(&worker);

    return bytes;
}

// Reads number of bytes written from received response.
static long written_bytes(conn worker)
{
    void *buffer = malloc(sizeof(struct packet) + 10);

    if (conn_read(worker, buffer, sizeof(struct packet) + 10) < 0)
    {
        free(buffer);
        return -1;
    }

    struct packet p = p_decode(buffer);
    long bytes = atol(p.arg);
    p_cleanup(p);

    return bytes;
}

// TODO: Just add this into write.
// TODO: Finish this.
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

// Deletes file from worker with that file.
void worker_delete(const char *file)
{
    struct file_server *worker = find_by_file(file);

    if (worker == NULL)
    {
#if defined(VERBOSE_1) || defined(VERBOSE_2)
        logger(ERROR, COMP_MASTER, "No workers with that file.");
#endif
        return;
    }

    struct packet p = p_init(0, file, DELETE);
    attach_token(MASTER_TKN, &p);

    conn worker_conn = client_init(worker->location.ip, worker->location.port);
    conn_write(worker_conn, p_encode(p), sizeof(p) + strlen(file));
    conn_close(&worker_conn);
}
