#ifndef SERVER_TABLE_H
#define SERVER_TABLE_H

typedef unsigned long ftid_t;

// Defines file server.
struct file_server
{
    ftid_t id;
    long size;

    struct
    {
        char *ip;
        unsigned short port;
    } location;
};

// Prototypes arguments are this way to ensure that added servers are never changed.
// If one is to change a server, it must be removed and then re-added.
void add_server(struct file_server server);
void remove_server(ftid_t id);
void for_each_server(void (*do)(struct fileserver *));
struct file_server *getfs_id(ftid_t id);
struct file_server *getfs_index(unsigned index);
struct file_server *get_pred(short (*pred)(struct file_server));

#endif
