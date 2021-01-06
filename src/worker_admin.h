#ifndef WORKER_ADMIN_H
#define WORKER_ADMIN_H

#include <pthread.h>
#include "server_table.h"

void set_admin_update_time(unsigned time);
unsigned get_admin_update_time();
pthread_t start_admin();
void add_worker(struct file_server worker);

// TODO: Add functions that manages all communication with workers.
unsigned short worker_count();
char *worker_ls(unsigned index);

#endif
