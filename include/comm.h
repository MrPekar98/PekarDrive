#ifndef COMM_H
#define COMM_H

#include <sys/types.h>

typedef struct
{
    short error;
    char *error_msg;
    int fd;
    unsigned long max_bytes;
} conn;

#ifdef __cplusplus
extern "C"
{
#endif
const char *get_ip();
unsigned short get_port();
unsigned short get_dbg_port();
conn client_init(const char *host, unsigned short port);
conn conn_listen(int server_fd, unsigned short port);
ssize_t conn_write(conn connection, const void *buffer, size_t len);
ssize_t conn_read(conn connection, void *buffer, size_t len);
void conn_close(conn *restrict connection);
#ifdef __cpluspls
}
#endif

#endif
