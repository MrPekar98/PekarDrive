#include <interface.h>
#include <stdio.h>
#include <string.h>
#include <comm.h>
#include "arg_parser.h"

#ifdef DEBUG
#define MASTER_ADDR "127.0.0.1"
#else
#define MASTER_ADDR "127.0.0.1"
#endif
#define MASTER_PORT get_port()

void operation_handler(const char *op, const char *arg1, const char *arg2);
void exec_ls();
void exec_read(const char *file);
void exec_write_append(const char *file, const char *data, short isappend);
void exec_delete(const char *file);

int main(int argc, char **argv)
{
    if (!parse_args(argv, argc))
    {
        printf("PekarDrive: %s\n", error_msg());
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
}

// Executes 'ls'.
void exec_ls()
{
#ifdef DEBUG
    printf("Executing 'ls'...\n");
#endif

    const void *res = ls(MASTER_ADDR, MASTER_PORT);

    if (res != NULL)
        printf("%s\n", res);
}

// Executes 'read'.
void exec_read(const char *file)
{
#ifdef DEBUG
    printf("Executing 'read'\n");
#endif

    const void *res = file_read(file, MASTER_ADDR, MASTER_PORT);

    if (res != NULL)
        printf("%s\n", res);
}

// Executes 'write' or 'append'.
void exec_write_append(const char *file, const char *data, short isappend)
{
#ifdef DEBUG
    printf("Executing '%s'...\n", isappend ? "append" : "write");
#endif

    size_t bytes = file_write(file, data, strlen(data), isappend, MASTER_ADDR, MASTER_PORT);

    if (res != NULL)
        printf("Wrote %d bytes\n", bytes);
}

// Executes 'delete'.
void exec_delete(const char *file)
{
#ifdef DEBUG
    printf("Executing 'delete'...\n");
#endif

    short deleted = file_delete(file, MASTER_ADDR, MASTER_PORT);

    if (res != NULL)
        printf("File was %s.\n", deleted ? "deleted" : "not deleted");
}
