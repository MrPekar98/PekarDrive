#ifndef WORKER_ADMIN_H
#define WORKER_ADMIN_H

#include <pthread.h>
#include "server_table.h"

void set_admin_update_time(unsigned time);
unsigned get_admin_update_time();
pthread_t start_admin();
void add_worker(struct file_server worker);
unsigned short worker_count();
char *worker_ls(unsigned index);
long worker_write(const char *file, const void *data);
long worker_append(const char *file, const void *data);
void *worker_read(const char *file);
void worker_delete(const char *file);

#endif
