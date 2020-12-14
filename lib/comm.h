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

// TODO: Missing initializer function.
conn conn_listen(int server_fd);
ssize_t conn_write(conn connection, const void *buffer, size_t len);
ssize_t conn_read(conn connection, void *buffer, size_t len);
void conn_close(conn *restrict connection);

#endif
