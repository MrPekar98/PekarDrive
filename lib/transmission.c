// TODO: Define functions to serialize and deserialize transmission struct.
// TODO: This will be used to transmit.

// TODO: Define local functions to execute the transmission protocol.
// TODO: comm.h should be used to send the data.

#include "transmission.h"
#include <stdlib.h>
#include <string.h>

static volatile unsigned short chunk_size = DEFAULT_CHUNK_SIZE;

// Constructor for struct endpoint.
struct endpoint make_endpoint(const char *ip, unsigned short port)
{
    struct endpoint e = {.ip = malloc(strlen(ip) + 1), .port = port};
    strcpy(e.ip, ip);

    return e;
}

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
transmission init_transmission(struct endpoint ep, const void *data, size_t size)
{
    transmission t = {.data = malloc(size), .size = size, .open = 1, .error = 0, .err_msg = NULL};
    memcpy(t.data, data, size);

    t.address.port = ep.port;
    t.address.ip = malloc(strlen(ep.ip) + 1);
    strcpy(t.address.ip, ep.ip);

    return t;
}

// Cleans up memory.
void close_transmission(transmission *restrict t)
{
    free(t->data);
    t->data = NULL;
    t->open = 0;
    t->size = 0;

    if (t->error && t->err_msg != NULL)
        free(t->err_msg);

    if (t->address.ip != NULL)
        free(t->address.ip);
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
    return t.size;
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
