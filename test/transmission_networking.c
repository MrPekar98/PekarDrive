#include "transmission_networking.h"
#include <signal.h>
#include <comm.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <PP/transmission.h>
#include <stdio.h>

#define TEST_PORT get_dbg_port()
#define ERROR_FILE "SERVER_ERROR"

// Prototypes.
static void start_server(unsigned port, int (*assert)(void *));
static inline void error(const char *msg);

// Starts server with debug port.
pid_t setup_server(int (*assert)(void *))
{
    pid_t server = fork();

    if (server < 0)
        return -1;

    else if (server == 0)
        start_server(TEST_PORT, assert);

    else
        return server;
}

// Server start.
// Receives messages and transmits them back again.
static void start_server(unsigned port, int (*assert)(void *))
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    conn client = conn_listen(fd, port);
    transmission read = init_transmission(client, NULL, 0), write;

    if (read.error)
    {
        error(transmission_error(read));
        return;
    }

    while (receive(&read))
    {
        if (read.error)
        {
            error(transmission_error(read));
            continue;
        }

        void *data = transmission_data(read);

        if (assert == NULL || !assert(data))
            write = init_transmission(client, "Assertion failed", 17);

        else
            write = init_transmission(client, "Assertion success", 17);

        transmit(&write);
        close_transmission(&write);
        close_transmission(&read);
        conn_close(&client);

        fd = socket(AF_INET, SOCK_STREAM, 0);
        client = conn_listen(fd, port);
        read = init_transmission(client, NULL, 0);
    }

    close_transmission(&read);
}

// Stops server.
void stop_server(pid_t pid)
{
    kill(pid, SIGKILL);
}

// Returns port used for testing.
unsigned short get_test_server_port()
{
    return TEST_PORT;
}

// Server error output.
static inline void error(const char *msg)
{
    FILE *error_file = fopen(ERROR_FILE, "w");

    if (error_file != NULL)
    {
        fprintf(error_file, "%s", msg);
        fclose(error_file);
    }
}
