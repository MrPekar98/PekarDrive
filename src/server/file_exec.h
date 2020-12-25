#ifndef FILE_EXEC_H
#define FILE_EXEC_H

#include <stddef.h>

#define FILE_READ 1
#define FILE_WRITE 2

struct file_output
{
    void *out;
    unsigned len;
};

struct file_list
{
    char **file_names;
    unsigned count;
};

struct file_output f_exec(short op, const void *arg, size_t len);
struct file_list file_list();

#endif
