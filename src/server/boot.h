#ifndef BOOT_H
#define BOOT_H

#ifdef DEBUG
#define WORKER_DIR "."
#define MASTER_PORT get_dbg_port()
#else
#define WORKER_DIR "~/.pekar_drive"
#define MASTER_PORT get_port()
#endif

short boot(const char *master_ip);
void terminate();

#endif
