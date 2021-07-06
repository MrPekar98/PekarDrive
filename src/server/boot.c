#include "boot.h"
#include "../fs/fs.h"
#include <comm.h>
#include <PP/packet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../util/logger.h"

// Prototypes.
static void data_init();
static short register_worker(const char *ip);

// Main entrance for booting worker.
short boot(const char *master_ip)
{
#ifdef LOG
    logger(MESSAGE, COMP_WORKER, "Worker booting...");
#endif

    data_init();
    return register_worker(master_ip);
}

// Creates hidden folder for data.
static void data_init()
{
    char *command = malloc(strlen(WORKER_DIR) + 10);
    sprintf(command, "mkdir %s\0", WORKER_DIR);

    system(command);
    free(command);

    set_dir(WORKER_DIR);
}

// Sends register message to master.
static short register_worker(const char *ip)
{
    conn worker = client_init(ip, MASTER_PORT);

    if (worker.error)
    {
#ifdef LOG
        logger(ERROR, COMP_WORKER, worker.error_msg);
#endif
        return 0;
    }

    unsigned bytes = 0;
    char *msg = malloc(21);
    sprintf(msg, "%s;%d", get_ip(), get_port());

    struct packet p = p_init(0, msg, REGISTER);
    attach_token(WORKER_TKN, &p);

    while ((bytes = conn_write(worker, p_encode(p), 40)) <= 0);
    free(msg);
    conn_close(&worker);

    return 1;
}

// Shutdown of worker.
void terminate()
{
#ifdef LOG
    logger(MESSAGE, COMP_WORKER, "Worker shutdown...");
#endif
}
