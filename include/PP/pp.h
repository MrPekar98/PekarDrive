#ifndef PP_H
#define PP_H

#include <stddef.h>
#include <comm.h>

typedef struct
{
    short error;            // Whether transmission has failed.
    char *error_message;    // Error message. If no error, this is NULL.
    size_t bytes;           // Number of transmitted bytes.
} pp_status;

typedef struct
{
    pp_status status;
    void *buffer;
} pp_message;

typedef struct
{
    conn connection;        // Connection instance.
} pp_endpoint;

pp_endpoint endpoint_from_address(const char *hostname, unsigned short port);
pp_endpoint endpoint_from_connection(conn connection);
pp_status pp_send(pp_endpoint endpoint, const void *data, size_t len);
pp_message pp_read(pp_endpoint endpoint);

#endif
