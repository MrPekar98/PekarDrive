#ifndef BOOT_H
#define BOOT_H

//#define WORKER_DIR "~/.pekar_worker"
#define WORKER_DIR "."
#define MASTER_ADDR "127.0.0.1"
//#define MASTER_PORT get_port()
#define MASTER_PORT 55442

short boot();
void terminate();

#endif
