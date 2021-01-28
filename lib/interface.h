#ifndef INTERFACE_H
#define INTERFACE_H

#include <stddef.h>

const void *ls(const char *host, unsigned short port);
const void *file_read(const char *file_name, const char *host, unsigned short port);
size_t file_write(const char *file_name, const void *buffer, size_t len, short append, const char *host, unsigned short port);
short file_delete(const char *file_name, const char *host, unsigned short port);

#endif
