#include "boot.h"
#include <comm.h>
#include <packet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Prototypes.
static void data_init();
static void register_worker();

// TODO: Make hidden folder if it does not already exist.
// Main entrance for booting worker.
void boot()
{
#ifdef LOG
    printf("Worker booting...\n");
#endif

    data_init();
    register_worker();
}

// Creates hidden folder for data.
static void data_init()
{
    char *command = malloc(strlen(WORKER_DIR) + 10);
    sprintf(command, "mkdir ~/%s\0", WORKER_DIR);

    system(command);
    free(command);
}

// Sends register message to master.
static void register_worker()
{
    conn worker = client_init(MASTER_ADDR, MASTER_PORT);
    unsigned bytes = 0;
    char *msg = malloc(21);
    sprintf(msg, "%s;%d", get_ip(), get_ip());

    struct packet p = p_init(0, msg, REGISTER);
    attach_token(WORKER_TKN, &p);

    while ((bytes = conn_write(worker, p_encode(p), 40)) <= 0);
    free(msg);
    conn_close(&worker);
}

// Shutdown of worker.
void terminate()
{
#ifdef LOG
    printf("Worker shutdown...\n");
#endif
}
