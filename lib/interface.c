#include "interface.h"
#include <string.h>
#include <stdlib.h>
#include <comm.h>
#include <packet.h>
#include <stdio.h>

static void *read_all(conn client);
static const char *gen_write_arg(const char *file, const char *data);

// Executes 'ls' command.
const void *ls(const char *host, unsigned short port)
{
    struct packet p = p_init(0, "", LS);
    conn client = client_init(host, port);

    if (client.error)
    {
#ifdef LOG
        printf("%s\n", client.error_msg);
#endif
        return NULL;
    }

    conn_write(client, p_encode(p), sizeof(p) + 3);

    void *buffer = read_all(client);
    conn_close(&client);

    return buffer;
}

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

    return buffer;
}

// Empties stream of bytes and returns it in a buffer.
static void *read_all(conn client)
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
    struct packet p = p_init(0, gen_write_arg(file_name, buffer), append ? APPEND : WRITE);
    conn client = client_init(host, port);

    if (client.error)
    {
#ifdef LOG
        printf("%s\n", client.error_msg);
#endif
        return -1;
    }

    conn_write(client, p_encode(p), sizeof(p) + strlen(p.arg) + 1);

    void *buffer_received = read_all(client);
    int written_bytes = atoi(buffer_received);
    conn_close(&client);
    free(buffer_received);

    return written_bytes;
}

// Generates argument for write and append.
static const char *gen_write_arg(const char *file, const char *data)
{
    char *arg = malloc(strlen(file) + strlen(data) + 3);
    sprintf(arg, "%s;%s\0", file, data);

    return arg;
}

// Deletes file if existing.
short file_delete(const char *file_name, const char *host, unsigned short port)
{
    struct packet p = p_init(0, file_name, DELETE);
    conn client = client_init(host, port);

    if (client.error)
    {
#ifdef LOG
        printf("%s\n", client.error_msg);
#endif
        return 0;
    }

    conn_write(client, p_encode(p), sizeof(p) + strlen(p.arg) + 1);

    void *buffer_received = read_all(client);
    int deleted = atoi(buffer_received);
    conn_close(&client);
    free(buffer_received);

    return deleted;
}
