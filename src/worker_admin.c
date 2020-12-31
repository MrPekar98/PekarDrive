#include "worker_admin.h"
#include "server_table.h"
#include <time.h>

static unsigned update_time = 60;   // Seconds between pinging workers.

static void *manager_thread(void *arg);
static void ping_all();

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
    pthread_create(thread, NULL, manager_thread, NULL);

    return thread;
}

// Thread for managing workers.
static void *manager_thread(void *arg)
{
    while (1)
    {
        time_t start = time(NULL);
        while (time(NULL) - start < update_time);

        start = time(NULL);
        ping_all();
    }
}


// TODO: What to do on failure?
// Sends ping to all workers.
static void ping_all()
{
    unsigned i, count = table_count();

    for (i = 0; i < count; i++)
    {

    }
}
