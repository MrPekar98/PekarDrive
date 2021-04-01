#include "transmission_networking.h"
#include <signal.h>
#include <comm.h>
#include <sys/socket.h>
#include <stdlib.h>

// Prototypes.
static void start_server(unsigned port, int (*assert)(void *));

// Starts server with debug port.
pid_t setup_server(int (*assert)(void *))
{
    pid_t server = fork();

    if (server < 0)
        return server;

    else if (server == 0)
        start_server(get_dbg_port(), assert);
}

// Server start.
// Receives messages and transmits them back again.
static void start_server(unsigned port, int (*assert)(void *))
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    conn client = conn_listen(fd, port);
    transmission read = init_transmission(client, NULL, 0), write;

    while (receive(&read))
    {
        if (assert != NULL && !assert(transmission_data(read)))
        {
            write = init_transmission(client, "Assertion failed.", 17);
            transmit(&write);
        }

        else
        {
            write = init_transmission(client, transmission_data(read), read.header.bytes);
            transmit(&write);
        }

        close_transmission(&write);
        close_transmission(&read);
        read = init_transmission(client, NULL, 0);
    }

    close_transmission(&read);
}

// Stops server.
void stop_server(pid_t pid)
{
    kill(pid, SIGKILL);
}
