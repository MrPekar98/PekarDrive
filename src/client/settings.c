#include "settings.h"
#include <stdio.h>
#include <stdlib.h>

static inline void set_master_address(const char *ip, unsigned short port);

// Stores address of master.
static inline void set_master_address(const char *ip, unsigned short port)
{
    FILE *f = fopen(CONFIG_FILE, "w");

    if (f == NULL)
        return;

    fprintf(f, "%s\n%d\n", ip == NULL ? "unknown" : ip, port);
    fclose(f);
}

// Saves IP address of master.
void set_master_host(const char *host)
{
    set_master_address(host, get_master_port());
}

// Saves port number of master.
void set_master_port(unsigned short port)
{
    set_master_address(get_master_host(), port);
}

// Getter to IP address of master.
// Returns NULL if nothing is set.
const char *get_master_host()
{
    FILE *f = fopen(CONFIG_FILE, "r");

    if (f == NULL)
        return NULL;

    char *ip = malloc(16);
    fscanf(f, "%[0-9.]", ip);
    fclose(f);

    return ip;
}

// Getter to port number of master.
// Returns 0 if nothing is set.
unsigned short get_master_port()
{
    FILE *f = fopen(CONFIG_FILE, "r");

    if (f == NULL)
        return 0;

    char *ip = malloc(16);
    unsigned port;
    fscanf(f, "%[0-9.A-Za-z]\n%d", ip, &port);
    free(ip);

    return port;
}
