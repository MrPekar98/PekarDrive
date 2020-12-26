#include "file_exec.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define SEP '#'

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Prototypes.
static const char *file_name(const char *ptr);
static const void *buffer(const void *ptr);
static struct file_output read_file(const char *file_name);
static struct file_output write_file(const char *file_name, const void *buffer);
static struct file_output append_file(const char *file_name, const void *buffer);

// Entrypoint for handling file operations.
// For write and append, arg has '#' as separator between filename and text to write.
struct file_output f_exec(short op, const void *arg)
{
    pthread_mutex_lock(&mutex);

    if (op & FILE_READ)
        return read_file(arg);

    if (op & FILE_WRITE)
        return write_file(file_name(arg), buffer(arg));

    if (op & FILE_APPEND)
        return append_file(file_name(arg), buffer(arg));

    pthread_mutex_unlock(&mutex);
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
    int byte;
    unsigned count = 0, growth_iter = 1;
    const unsigned grow = 3;
    struct file_output output = {.len = 0, .out = malloc(grow)};
    FILE *f = fopen(file_name, "r");

    if (f == NULL)
    {
        output.error = 1;
        free(output.out);
        return output;
    }

    while ((byte = fgetc(f)) != EOF)
    {
        if (count + 1 == grow)
            output.out = realloc(output.out, grow * ++growth_iter);

        memcpy(output.out + count++, &byte, sizeof(char));
        output.len++;
    }

    fclose(f);
    return output;
}

// Writes to new file.
// Old file with same name is overwritten.
static struct file_output write_file(const char *file_name, const void *buffer)
{
    FILE *f = fopen(file_name, "w");
    fwrite(buffer, sizeof(char), strlen((char *) buffer), f);
    fclose(f);
}

// Appends buffer to existing file.
// If does not exist, create new.
static struct file_output append_file(const char *file_name, const void *buffer)
{
    FILE *f = fopen(file_name, "a");
    fwrite(buffer, sizeof(char), strlen((char *) buffer), f);
    fclose(f);
}

// Returns a list of files names.
struct file_list file_list()
{
    pthread_mutex_lock(&mutex);

    DIR *directory = opendir("./");
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
