// TODO: Define functions to serialize and deserialize transmission struct.
// TODO: This will be used to transmit.

// TODO: Define local functions to execute the transmission protocol.
// TODO: comm.h should be used to send the data.

#include "transmission.h"
#include <stdlib.h>
#include <string.h>

static volatile unsigned short chunk_size = DEFAULT_CHUNK_SIZE;

// Setter to chunk size.
void set_transmission_chunk_size(unsigned short size)
{
    chunk_size = size;
}

// Getter to chunk size.
unsigned short get_transmission_chunk_size()
{
    return chunk_size;
}

// Creates transmission instance.
transmission init_transmission(conn connection, const void *data, size_t size)
{
    transmission t = {.data = malloc(size), .open = 1, .error = 0, .err_msg = NULL, .error_len = 0};
    memcpy(t.data, data, size);

    t.header.bytes = size;
    t.header.chunk_size = chunk_size;
    t.connection.error = connection.error;
    t.connection.fd = connection.fd;
    t.connection.max_bytes = connection.max_bytes;

    if (connection.error && connection.error_msg != NULL)
    {
        t.connection.error_msg = malloc(strlen(connection.error_msg) + 1);
        strcpy(t.connection.error_msg, connection.error_msg);
    }

    return t;
}

// Cleans up memory.
// Connection is kept open.
void close_transmission(transmission *restrict t)
{
    free(t->data);
    t->data = NULL;
    t->open = 0;
    t->header.bytes = 0;

    if (t->error && t->err_msg != NULL)
        free(t->err_msg);
}

// Returns error message from transmission.
const char *transmission_error(transmission t)
{
    if (!t.error)
        return NULL;

    return t.err_msg;
}

// Returns size of transmission data.
size_t transmission_size(transmission t)
{
    return t.header.bytes;
}

// TODO: Errors are inserted into argument transmission.
// Entry function for transmitting message.
char transmit(transmission *restrict t)
{
    return 1;
}

// TODO: Received data inserted into argument transmission.
// Entry function for reading transmitted data.
char receive(transmission *restrict t)
{
    return 1;
}

// Returns data.
void *transmission_data(transmission t)
{
    return t.data;
}
