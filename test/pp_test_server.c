#include "pp_test_server.h"
#include <sys/socket.h>
#include <signal.h>
#include <PP/pp.h>

#define PORT 48555

static void run_server();

// Server responds with "Success" on success and "Fail" on failure.
pid_t pp_start_server()
{
    pid_t pid = fork();

    if (pid == 0)
        run_server();

    else if (pid > 0)
        return pid;

    else
        return -1;
}

static void run_server()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    conn client = conn_listen(fd, PORT);
    pp_endpoint endpoint = endpoint_from_connection(client);

    while (1)
    {
        pp_message message = pp_read(endpoint);

        if (message.status.error)
            pp_send(endpoint, "Fail", 4);

        else
            pp_send(endpoint, "Success", 7);

        break;
    }
}

void pp_stop_server(pid_t pid)
{
    kill(pid, SIGKILL);
}

unsigned short pp_test_server_port()
{
    return PORT;
}
