#include "arg_parser.h"
#include <stdlib.h>
#include <string.h>

#define PARSE_ERR(msg) set_err_msg(msg)

static char *parse_error_msg = NULL;

// Prototypes.
static void set_err_msg(const char *msg);
static short parse_operation(char **args, unsigned short arg_count);
static short parse_ls(unsigned short arg_count);
static short parse_read(char **args, unsigned short arg_count);
static short parse_write(char **args, unsigned short arg_count);
static short parse_append(char **args, unsigned short arg_count);
static short parse_delete(char **args, unsigned short arg_count);
static short parse_set_ip(char **args, unsigned short arg_count);
static short parse_set_port(char **args, unsigned short arg_count);

// Setter for error message.
static void set_err_msg(const char *msg)
{
    if (parse_error_msg != NULL)
        free(parse_error_msg);

    parse_error_msg = malloc(strlen(msg) + 1);
    strcpy(parse_error_msg, msg);
}

// Parses main arguments.
short parse_args(char **argv, int argc)
{
    if (argc < 2)
    {
        PARSE_ERR("Not enough arguments.");
        return 0;
    }

    return parse_operation(argv, argc);
}

// Returns error message.
const char *error_msg()
{
    return parse_error_msg;
}

// Parses first argument: operation.
static short parse_operation(char **args, unsigned short arg_count)
{
    if (strcmp(args[1], "ls") == 0)
        return parse_ls(arg_count);

    else if (strcmp(args[1], "read") == 0)
        return parse_read(args, arg_count);

    else if (strcmp(args[1], "write") == 0)
        return parse_write(args, arg_count);

    else if (strcmp(args[1], "append") == 0)
        return parse_append(args, arg_count);

    else if (strcmp(args[1], "delete") == 0)
        return parse_delete(args, arg_count);

    else if (strcmp(args[1], "set-ip") == 0)
        return parse_set_ip(args, arg_count);

    else if (strcmp(args[1], "set-port") == 0)
        return parse_set_port(args, arg_count);

    PARSE_ERR("Unrecognized operation.");
    return 0;
}

// Parses 'ls' operation.
static short parse_ls(unsigned short arg_count)
{
    if (arg_count > 2)
        PARSE_ERR("No arguments to 'ls'needed.");

    return 1;
}

// Parses 'read' operation.
static short parse_read(char **args, unsigned short arg_count)
{
    if (arg_count < 3)
    {
        PARSE_ERR("Missing file name to read from.");
        return 0;
    }

    else if (arg_count > 3)
        PARSE_ERR("'read' requires only one argument.");

    return 1;
}

// Parses 'write' operation.
static short parse_write(char **args, unsigned short arg_count)
{
    if (arg_count < 4)
    {
        PARSE_ERR("'write' requires file name and data to write.");
        return 0;
    }

    else if (arg_count > 4)
        PARSE_ERR("'write' requires only two arguments.");

    return 1;
}

// Parses 'append' operation.
static short parse_append(char **args, unsigned short arg_count)
{
    if (arg_count < 4)
    {
        PARSE_ERR("'append' requires file name and data to write.");
        return 0;
    }

    else if (arg_count > 4)
        PARSE_ERR("'write' requires only two arguments.");

    return 1;
}

// Parses 'delete' operation.
static short parse_delete(char **args, unsigned short arg_count)
{
    if (arg_count < 3)
    {
        PARSE_ERR("Missing file name to delete.");
        return 0;
    }

    else if (arg_count > 3)
        PARSE_ERR("'delete' requires only one argument.");

    return 1;
}

// Parses 'set-ip' command.
static short parse_set_ip(char **args, unsigned short arg_count)
{
    if (arg_count < 3)
    {
        PARSE_ERR("Missing IP address.");
        return 0;
    }

    else if (arg_count > 3)
        PARSE_ERR("'set-ip' requires only one argument.");

    return 1;
}

// Parses 'set-port' command-
static short parse_set_port(char **args, unsigned short arg_count)
{
    if (arg_count < 3)
    {
        PARSE_ERR("Missing port number.");
        return 0;
    }

    else if (arg_count > 3)
        PARSE_ERR("'set-port' requires only one argument.");

    return 1;
}
