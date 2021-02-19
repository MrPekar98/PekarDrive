#include "comm.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

#define ERR_MSG_LEN 50
#define PORT 55443
#define DEBUG_PORT 55442

#define READ_TIMEOUT 4

// Returns error conn instance with error message.
#define ERR_CONN(msg, len) ({ \
    conn c = {.error = 1, .error_msg = malloc(len)}; \
    sprintf(c.error_msg, "%s\0", msg); \
    c; \
})

// Returns IP.
const char *get_ip()
{
#ifdef DEBUG
    return "127.0.0.1";
#else
    FILE *prog = popen("hostname -I", "r");
    char *host = malloc(16);
    fscanf(prog, "%[0-9.]", host);
    fclose(prog);

    return host;
#endif
}

// Returns port.
unsigned short get_port()
{
    return PORT;
}

// Returns alternative port for debugging.
unsigned short get_dbg_port()
{
    return DEBUG_PORT;
}

// Initializer for client.
conn client_init(const char *host, unsigned short port)
{
    struct sockaddr_in server_addr;
    conn client = {.error_msg = ""};
    struct timeval timeout = {.tv_sec = READ_TIMEOUT, .tv_usec = 0};

    if ((client.fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return ERR_CONN("Failed initialising socket.", ERR_MSG_LEN);

    else if (setsockopt(client.fd, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout)) < 0 ||
                setsockopt(client.fd, SOL_SOCKET, SO_SNDTIMEO, (char *) &timeout, sizeof(timeout)) < 0)
        return ERR_CONN("Failed setting socket option.", ERR_MSG_LEN);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0)
        return ERR_CONN("Invalid address or address not supported.", ERR_MSG_LEN);

    else if (connect(client.fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
        return ERR_CONN("Connection failed.", ERR_MSG_LEN);

    return client;
}

// Listens for client trying to connect.
conn conn_listen(int server_fd, unsigned short port)
{
#ifdef LOG
    printf("Waiting for connection on port %d...\n", port);
#endif

    struct sockaddr_in addr;
    size_t addr_len = sizeof(addr);
    int opt = 1;
    conn connection = {.error = 0};

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        return ERR_CONN("Failed setting socket option.", ERR_MSG_LEN);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
        return ERR_CONN("Failed binding.", ERR_MSG_LEN);

    else if (listen(server_fd, 10) < 0)
        return ERR_CONN("Failed listening.", ERR_MSG_LEN);

    else if ((connection.fd = accept(server_fd, (struct sockaddr *) &addr, (socklen_t *) &addr_len)) < 0)
        return ERR_CONN("Failed accepting client.", ERR_MSG_LEN);

    return connection;
}

// Writes message to file descriptor.
ssize_t conn_write(conn connection, const void *buffer, size_t len)
{
#ifdef DEBUG
    printf("Writing to %d...\n", connection.fd);
#endif

    return send(connection.fd, buffer, len, 0);
}

// Reads message from file descriptor.
ssize_t conn_read(conn connection, void *buffer, size_t len)
{
#ifdef DEBUG
    printf("Reading from %d...\n", connection.fd);
#endif

    return read(connection.fd, buffer, len);
}

// Closes connection.
void conn_close(conn *restrict connection)
{
#ifdef DEBUG
    printf("Closing %d...\n", connection->fd);
#endif

    close(connection->fd);
    *connection = ERR_CONN("Connection closed.", ERR_MSG_LEN);
}
