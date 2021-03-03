#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include <stddef.h>

#define DEFAULT_CHUNK_SIZE 200

// Endpoint struct defining the address of communication.
struct endpoint
{
    char *ip;
    unsigned short port;
};

// Main transmission struct.
typedef struct
{
    void *data;
    size_t size;
    char open, error;
    char *err_msg;

    struct _header
    {
        size_t bytes;
        unsigned chunk_size;
    } header;

    struct endpoint address;
} transmission;

struct endpoint make_endpoint(const char *ip, unsigned short port);
void set_transmission_chunk_size(unsigned short size);
unsigned short get_transmission_chunk_size();
transmission init_transmission(struct endpoint ep, const void *data, size_t size);
void close_transmission(transmission *restrict t);
const char *transmission_error(transmission t);
size_t transmission_size(transmission t);
char transmit(transmission *restrict t);
char receive(transmission *restrict t);

#endif
