#include "server_table.h"
#include <stdlib.h>

static struct file_server *server_table = NULL;
static unsigned short server_count = 0;

// Prototypes.
static inline int indexof(ftid_t id);

// Adds server to table.
void add_server(struct file_server server)
{
    server_table = (struct file_server *) realloc(server_table, sizeof(struct file_server) * ++server_count);
    server_table[server_count - 1] = server;
}

// Removed file server from table.
void remove_server(ftid_t id)
{
    int file_server_idx = indexof(id);

    if (file_server_idx < 0)
        return;

    for (; file_server_idx + 1 < server_count; file_server_idx++)
    {
        server_table[file_server_idx] = server_table[file_server_idx + 1];
    }

    server_table = (struct file_server *) realloc(server_table, sizeof(struct file_server) * --server_count);
}

// Finds index of file server with given ID.
static inline int indexof(ftid_t id)
{
    unsigned i;

    for (i = 0; i < server_count; i++)
    {
        if (server_table[i].id == id)
            return i;
    }

    return -1;
}

// For-each loop of server table.
void for_each_server(void (*apply)(struct file_server *))
{
    unsigned i;

    for (i = 0; i < server_count; i++)
    {
        apply(&server_table[i]);
    }
}

// Returns file server with given id.
struct file_server *getfs_id(ftid_t id)
{
    unsigned i;

    for (i = 0; i < server_count; i++)
    {
        if (server_table[i].id == id)
            return &server_table[i];
    }

    return NULL;
}

// Returns file server from given index in table.
struct file_server *getfs_index(unsigned index)
{
    if (index >= server_count)
        return NULL;

    return &server_table[index];
}

// Returns first file_server that satisfies given predicate.
struct file_server *getfs_pred(short (*pred)(struct file_server))
{
    unsigned i;

    for (i = 0; i < server_count; i++)
    {
        if (pred(server_table[i]))
            return &server_table[i];
    }

    return NULL;
}

// Returns count of tables.
unsigned short table_count()
{
    return server_count;
}
