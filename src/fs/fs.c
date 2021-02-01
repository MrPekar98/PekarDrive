#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define READ_SIZE 100

static char *work_dir = NULL;

// Prototypes.
static short file_exists(const char *file);
static inline const char *set_path(const char *file_name);

// Sets directory to work in.
void set_dir(const char *dir)
{
    work_dir = malloc(strlen(dir) + 1);
    strcpy(work_dir, dir);
}

// Creates a new file if it doesn't already exist.
short fs_create_file(const char *file)
{
    FILE *f = fopen(set_path(file), "w");
    short ret = f != NULL;

    if (f != NULL)
        fclose(f);

    return ret;
}

// Deletes a file if it exists.
void fs_delete_file(const char *file)
{
    const char *path = set_path(file);

    if (file == NULL || !file_exists(path))
        return;

    char *command = malloc(strlen(path) + 4);
    sprintf(command, "rm %s", path);
    system(command);
    free(command);
}

// Writes to file.
long fs_write_file(const char *file, const void *buffer, unsigned long size, short isappend)
{
    const char *path = set_path(file);

    if (file == NULL || (isappend && !file_exists(path)))
        return -1;

    else if (!file_exists(path))
        fs_create_file(path);

    FILE *f = fopen(path, isappend ? "a" : "w");

    if (f == NULL)
    {
#ifdef LOG
        perror("File operation error");
#endif
        return -1;
    }

    unsigned long written = fprintf(f, "%s", (char *) buffer);
    fclose(f);

    return written;
}

// Reads all content in file.
void *fs_read_file(const char *file)
{
    FILE *f = fopen(set_path(file), "r");

    if (f == NULL)
    {
#ifdef LOG
        perror("File operation error");
#endif
        return NULL;
    }

    char *buffer = malloc(READ_SIZE);
    int c;
    unsigned long counter = 0;

    while ((c = getc(f)) != EOF)
    {
        if (c % READ_SIZE == 0)
            buffer = realloc(buffer, READ_SIZE + counter);

        buffer[counter++] = c;
    }

    return buffer;
}

// Checks whether a given file exists.
static short file_exists(const char *file)
{
    FILE *f = fopen(file, "r");

    if (f == NULL)
        return 0;

    fclose(f);
    return 1;
}

// Appends working directory to file name if set.
static inline const char *set_path(const char *file_name)
{
    if (file_name == NULL)
        return NULL;

    else if (work_dir == NULL)
        return file_name;

    char *appended = malloc(strlen(work_dir) + strlen(file_name) + 2);
    sprintf(appended, "%s/%s", work_dir, file_name);

    return appended;
}
