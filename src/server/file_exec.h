#ifndef FILE_EXEC_H
#define FILE_EXEC_H

#define FILE_READ 1
#define FILE_WRITE 2
#define FILE_APPEND 4
#define FILE_DELETE 8

struct file_output
{
    short error;
    void *out;
    unsigned len;
};

struct file_list
{
    char **file_names;
    unsigned count;
};

struct file_output f_exec(short op, const void *arg);
struct file_list file_list(const char *dir);

#endif
