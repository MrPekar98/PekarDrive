#include "transmission.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TRANS_ERR(trans, err) ({ \
    close_transmission(trans); \
    trans->error_len = strlen(err); \
    trans->err_msg = malloc(trans->error_len); \
    strcpy(trans->err_msg, err); \
    *trans; \
})

static volatile unsigned short chunk_size = DEFAULT_CHUNK_SIZE;

// Prototypes.
static inline void fill_buffer(void *buffer, const void *data, unsigned buffer_size, unsigned data_size);
static inline unsigned trim_padding(void *buffer, unsigned size);

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
    transmission t = {.read_used = 0, .write_used = 0, .data = malloc(size), .open = 1, .error = 0,
                        .err_msg = NULL, .error_len = 0};
    memcpy(t.data, data, size);

    t.header.bytes = size;
    t.header.chunk_size = chunk_size;
    t.connection.error = connection.error;
    t.connection.fd = connection.fd;
    t.connection.max_bytes = connection.max_bytes;

    if (connection.error && connection.error_msg != NULL)
    {
        t.error = 1;
        t.connection.error_msg = malloc(strlen(connection.error_msg) + 1);
        strcpy(t.connection.error_msg, connection.error_msg);
    }

    if (t.header.chunk_size <= SERIALIZED_SIZE_WITHOUT_DATA)
    {
        t.error = 1;
        t.err_msg = malloc(50);
        strcpy(t.err_msg, "Chunk size too small for transmission.");
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

// Entry function for transmitting message.
char transmit(transmission *restrict t)
{
    if (t->write_used)
    {
        *t = TRANS_ERR(t, "Transmission instance already used for writing.");
        return 0;
    }

    else if (t->error)
    {
        *t = TRANS_ERR(t, "Erroneous transmission instance used.");
        return 0;
    }

    else if (!t->open)
    {
        *t = TRANS_ERR(t, "Transmission instance is closed.");
        return 0;
    }

    t->write_used = 1;
    void *buffer = NULL;
    unsigned i, data_size = t->header.chunk_size - SERIALIZED_SIZE_WITHOUT_DATA,
                    iterations = floor((double) t->header.bytes / data_size),
                    remaining = t->header.bytes % data_size;

    for (i = 0; i < iterations; i++)
    {
        buffer = malloc(data_size);
        fill_buffer(buffer, t->data + i * data_size, data_size, data_size);

        transmission sub = init_transmission(t->connection, buffer, data_size);
        void *serialized = transmission_serialize(sub);
        conn_write(t->connection, buffer, t->header.chunk_size);

        close_transmission(&sub);
        free(buffer);
        free(serialized);
    }

    buffer = malloc(data_size);
    fill_buffer(buffer, t->data + i * data_size, data_size, remaining);
    transmission sub = init_transmission(t->connection, buffer, data_size);
    void *serialized = transmission_serialize(sub);
    conn_write(t->connection, buffer, t->header.chunk_size);

    close_transmission(&sub);
    free(buffer);
    free(serialized);
}

// Copies data into buffer.
// If less data given that buffer can contain, padd with value 4.
static inline void fill_buffer(void *buffer, const void *data, unsigned buffer_size, unsigned data_size)
{
    if (buffer_size < data_size)
        return;

    memcpy(buffer, data, data_size);
    unsigned i;
    char *sub_dest = (char *) (buffer + data_size);

    for (i = 0; i < buffer_size - data_size; i++)
    {
        sub_dest[i] = 4;
    }
}

// Entry function for reading transmitted data.
char receive(transmission *restrict t)
{
    if (t->read_used)
    {
        *t = TRANS_ERR(t, "Transmission instance already used for reading.");
        return 0;
    }

    else if (t->error)
    {
        *t = TRANS_ERR(t, "Erroneous transmission instance used.");
        return 0;
    }

    else if (!t->open)
    {
        *t = TRANS_ERR(t, "Transmission instance is closed.");
        return 0;
    }

    t->read_used = 1;
    void *buffer = malloc(t->header.chunk_size);
    unsigned i = 0;

    while (conn_read(t->connection, buffer + (i++ * t->header.chunk_size), t->header.chunk_size) > 0)
    {
        buffer = realloc(buffer, (i + 1) * t->header.chunk_size);
    }

    unsigned new_size = trim_padding(buffer, (i - 1) * t->header.chunk_size);

    if (t->data != NULL)
        free(t->data);

    t->data = malloc(new_size);
    memcpy(t->data, buffer, new_size);
    free(buffer);

    return 1;
}

// Trims buffer from ending padding values.
static inline unsigned trim_padding(void *buffer, unsigned size)
{
    char *bytes = (char *) buffer;

    for (size--; size >= 0; size--)
    {
        if (bytes[size] != 4)
            break;
    }

    return size + 1;
}

// Returns data.
void *transmission_data(transmission t)
{
    return t.data;
}
