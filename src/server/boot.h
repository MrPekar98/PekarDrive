#ifndef BOOT_H
#define BOOT_H

#define WORKER_DIR "~/.pekar_drive"
#define MASTER_PORT get_port()

short boot(const char *master_ip);
void terminate();

#endif
