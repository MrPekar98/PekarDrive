#include "server_table.h"
#include <stdlib.h>

static struct file_server *server_table = NULL;
static unsigned short server_count = 0;

// Adds server to table.
void add_server(struct file_server server)
{
    server_table = (struct file_server *) realloc(server_table, sizeof(struct file_server) * ++server_count);
    server_table[server_count - 1] = server;
}

// Removed file server from table.
void remove_server(ftid_t id)
{

}

// For-each loop of server table.
void for_each_server(void (*do)(struct fileserver *))
{
    unsigned i;

    for (i = 0; i < server_count; i++)
    {
        do(&server_table[i]);
    }
}
