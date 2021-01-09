#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define READ_SIZE 100

// Prototypes.
static short file_exists(const char *file);

// Creates a new file if it doesn't already exist.
short fs_create_file(const char *file)
{
    FILE *f = fopen(file, "w");
    short ret = f != NULL;

    if (f != NULL)
        fclose(f);

    return ret;
}

// Deletes a file if it exists.
void fs_delete_file(const char *file)
{
    if (file == NULL || !file_exists(file))
        return;

    char *command = malloc(strlen(file) + 4);
    sprintf(command, "rm %s", file);
    system(command);
    free(command);
}

// Writes to file.
long fs_write_file(const char *file, void *buffer, unsigned long size, short isappend)
{
    if (file == NULL || !file_exists(file))
        return -1;

    FILE *f = fopen(file, isappend ? "a" : "w");
    unsigned long written = fprintf(f, "%s", (char *) buffer);
    fclose(f);

    return written;
}

// Reads all content in file.
void *fs_read_file(const char *file)
{
    FILE *f = fopen(file, "r");
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
