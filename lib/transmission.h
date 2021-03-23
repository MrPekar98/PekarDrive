#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include "comm.h"
#include <stddef.h>

#define DEFAULT_CHUNK_SIZE 200

extern unsigned SERIALIZED_SIZE_WITHOUT_DATA;

// Main transmission struct.
// Same transmission instance can only be used once for transmitting and receiving.
typedef struct
{
    void *data;
    char open, error;
    char *err_msg;
    size_t error_len;

    struct _header
    {
        size_t bytes;
        unsigned chunk_size;
    } header;

    conn connection;

    // Not serialized.
    short read_used;
    short write_used;
} transmission;

#ifdef __cplusplus
extern "C"
{
#endif
void set_transmission_chunk_size(unsigned short size);
unsigned short get_transmission_chunk_size();
transmission init_transmission(conn connection, const void *data, size_t size);
void close_transmission(transmission *restrict t);
const char *transmission_error(transmission t);
size_t transmission_size(transmission t);
char transmit(transmission *restrict t);
char receive(transmission *restrict t);
void *transmission_data(transmission t);
void *transmission_serialize(transmission t);
transmission transmission_deserialize(const void *serialization);
#ifdef __cplusplus
}
#endif

#endif
