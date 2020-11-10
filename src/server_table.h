#ifndef SERVER_TABLE_H
#define SERVER_TABLE_H

// Defines file server.
struct file_server
{
    long size;

    struct
    {
        char *ip;
        unsigned short port;
    } location;
};

#endif
