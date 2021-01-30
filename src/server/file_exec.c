#include "file_exec.h"
#include "../fs/fs.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stddef.h>

#define SEP ';'

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Prototypes.
static const char *file_name(const char *ptr);
static const void *buffer(const void *ptr);
static struct file_output read_file(const char *file_name);
static struct file_output write_file(const char *file_name, const void *buffer);
static struct file_output append_file(const char *file_name, const void *buffer);
static struct file_output delete_file(const char *file_name);

// Entrypoint for handling file operations.
// For write and append, arg has '#' as separator between filename and text to write.
struct file_output f_exec(short op, const void *arg)
{
    struct file_output output;
    pthread_mutex_lock(&mutex);

    if (op & FILE_READ)
        output = read_file(arg);

    if (op & FILE_WRITE)
        output = write_file(file_name(arg), buffer(arg));

    if (op & FILE_APPEND)
        output = append_file(file_name(arg), buffer(arg));

    if (op & FILE_DELETE)
        output = delete_file(arg);

    pthread_mutex_unlock(&mutex);
    return output;
}

// Exctacts file_name from string format.
static const char *file_name(const char *ptr)
{
    unsigned counter = 0;
    while (ptr[counter++] != SEP);

    char *name = malloc(counter + 1);
    strncpy(name, ptr, counter - 1);
    return name;
}

// Extracts argument in string format.
static const void *buffer(const void *ptr)
{
    unsigned counter = 0;
    const char *byte_stream = ptr;
    while (byte_stream[counter++] != SEP);

    return ptr + counter;
}

// Reads file.
static struct file_output read_file(const char *file_name)
{
    void *read = fs_read_file(file_name);
    unsigned read_len = strlen((char *) read);
    struct file_output output = {.len = read_len, .out = malloc(read_len + 1), .error = 0};
    memcpy(output.out, read, read_len);

    return output;
}

// Writes to new file.
// Old file with same name is overwritten.
static struct file_output write_file(const char *file_name, const void *buffer)
{
    long bytes = fs_write_file(file_name, buffer, strlen((char *) buffer), 0);
    return (struct file_output) {.error = 0, .out = &bytes, .len = sizeof(long)};
}

// Appends buffer to existing file.
// If does not exist, create new.
static struct file_output append_file(const char *file_name, const void *buffer)
{
    long bytes = fs_write_file(file_name, buffer, strlen((char *) buffer), 1);
    return (struct file_output) {.error = 0, .out = &bytes, .len = sizeof(long)};
}

// Returns a list of files names.
struct file_list file_list(const char *dir)
{
    pthread_mutex_lock(&mutex);

    DIR *directory = opendir(dir);
    struct file_list list = {.count = 0};
    struct dirent *d;

    while ((d = readdir(directory)) != NULL)
    {
        list.file_names = (char **) realloc(list.file_names, sizeof(char *) * (list.count + 1));
        list.file_names[list.count] = malloc(strlen(d->d_name));
        strcpy(list.file_names[list.count++], d->d_name);
    }

    pthread_mutex_unlock(&mutex);
    return list;
}

// Deletes file.
static struct file_output delete_file(const char *file_name)
{
    fs_delete_file(file_name);
    return (struct file_output) {.error = 0, .out = NULL, .len = 0};
}
