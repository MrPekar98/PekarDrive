#include <interface.h>
#include <stdio.h>
#include <string.h>
#include <comm.h>
#include <stdlib.h>
#include "arg_parser.h"
#include "settings.h"
#include "../util/logger.h"

#define MASTER_ADDR get_master_host()
#define MASTER_PORT get_master_port()

void operation_handler(const char *op, const char *arg1, const char *arg2);
void exec_ls();
void exec_read(const char *file);
void exec_write_append(const char *file, const char *data, short isappend);
void exec_delete(const char *file);
void exec_set_ip(const char *ip);
void exec_set_port(unsigned short port);

int main(int argc, char **argv)
{
    if (!parse_args(argv, argc))
    {
        logger(ERROR, COMP_CLIENT, error_msg());
        return 1;
    }

    else if (strcmp(argv[1], "set-ip") != 0 && strcmp(argv[1], "set-port") != 0 && get_master_host() == NULL)
    {
        logger(MESSAGE, COMP_CLIENT, "Use 'set-ip' and 'set-port' to specify master IP and port number, repectively.");
        return 1;
    }

    operation_handler(argv[1], argc >= 3 ? argv[2] : NULL, argc >= 4 ? argv[3] : NULL);

    return 0;
}

// Argument handler.
void operation_handler(const char *op, const char *arg1, const char *arg2)
{
    if (strcmp(op, "ls") == 0)
        exec_ls();

    else if (strcmp(op, "read") == 0)
        exec_read(arg1);

    else if (strcmp(op, "write") == 0 || strcmp(op, "append") == 0)
        exec_write_append(arg1, arg2, strcmp(op, "append") == 0);

    else if (strcmp(op, "delete") == 0)
        exec_delete(arg1);

    else if (strcmp(op, "set-ip") == 0)
        exec_set_ip(arg1);

    else if (strcmp(op, "set-port") == 0)
        exec_set_port(atoi(arg1));
}

// Executes 'ls'.
void exec_ls()
{
#ifdef DEBUG
    logger(MESSAGE, COMP_CLIENT, "Executing 'ls'...");
#endif

    const void *res = ls(MASTER_ADDR, MASTER_PORT);

    if (res != NULL)
        printf("%s\n", (char *) res);
}

// Executes 'read'.
void exec_read(const char *file)
{
#ifdef DEBUG
    logger(MESSAGE, COMP_CLIENT, "Executing 'read'...");
#endif

    const void *res = file_read(file, MASTER_ADDR, MASTER_PORT);

    if (res != NULL)
        printf("%s\n", res);
}

// Executes 'write' or 'append'.
void exec_write_append(const char *file, const char *data, short isappend)
{
#ifdef DEBUG
    if (isappend)
        logger(MESSAGE, COMP_CLIENT, "Executing 'append'...");

    else
        logger(MESSAGE, COMP_CLIENT, "Executing 'write'...");
#endif

    size_t bytes = file_write(file, data, strlen(data), isappend, MASTER_ADDR, MASTER_PORT);
    printf("Wrote %d bytes\n", bytes);
}

// Executes 'delete'.
void exec_delete(const char *file)
{
#ifdef DEBUG
    logger(MESSAGE, COMP_CLIENT, "Executing 'delete'...");
#endif

    short deleted = file_delete(file, MASTER_ADDR, get_master_port());
    printf("File was %s.\n", deleted ? "deleted" : "not deleted");
}

// Executes 'set-ip'.
void exec_set_ip(const char *ip)
{
    set_master_host(ip);
}

// Executes 'set-port'.
void exec_set_port(unsigned short port)
{
    set_master_port(port);
}
