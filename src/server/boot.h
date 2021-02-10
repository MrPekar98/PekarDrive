#ifndef BOOT_H
#define BOOT_H

#define WORKER_DIR "~/.pekar_drive"
#define MASTER_ADDR "127.0.0.1"
#define MASTER_PORT get_port()

short boot();
void terminate();

#endif
