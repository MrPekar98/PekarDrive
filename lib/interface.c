#include "interface.h"
#include <string.h>
#include <stdlib.h>
#include <comm.h>
#include <packet.h>
#include <stdio.h>

void *read_all(conn client);

// Reads all bytes of file.
const void *file_read(const char *file_name, const char *host, unsigned short port)
{
    struct packet p = p_init(0, file_name, READ);
    conn client = client_init(host, port);

    if (client.error)
    {
#ifdef LOG
        printf("%s\n", client.error_msg);
#endif
        return NULL;
    }

    conn_write(client, p_encode(p), sizeof(p) + strlen(file_name));
    void *buffer = read_all(client);
    conn_close(&client);

    return p_decode(buffer).arg;
}

// Empties stream of bytes and returns it in a buffer.
void *read_all(conn client)
{
    const unsigned short read_size = 1000;
    unsigned i = 0;
    void *buffer = malloc(read_size);

    while (conn_read(client, buffer + i * read_size, read_size) == read_size)
    {
        buffer = realloc(buffer, read_size * (++i + 1));
    }

    return buffer;
}

// Writes bytes to file.
size_t file_write(const char *file_name, const void *buffer, size_t len, short append, const char *host, unsigned short port)
{
    return 0;
}

// Deletes file if existing.
short file_delete(const char *file_name, const char *host, unsigned short port)
{
    return 0;
}
