#include "../src/server/file_exec.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

void test_file_list();
void test_file_read();
void test_file_write();
void test_file_append();

int main()
{
    test_file_list();
    test_file_read();
    test_file_write();
    test_file_append();

    return 0;
}

// Tests listing all file names.
void test_file_list()
{
    system("touch file1");
    system("touch file2");
    system("touch file3");

    struct file_list ls = file_list();
    assert(ls.count == 3);
    assert(strcmp(ls.file_names[0], "file1") == 0);
    assert(strcmp(ls.file_names[1], "file2") == 0);
    assert(strcmp(ls.file_names[2], "file3") == 0);

    system("rm file1 file2 file3");
}

// Tests reading file.
void test_file_read()
{
    system("touch file");
    system("echo Hello, World. > file");

    struct file_output fo = f_exec(FILE_READ, "file");
    assert(!fo.error);
    assert(fo.len == 14);
    //assert(strcmp((char *) fo.out, "Hello, World\n\0") == 0);

    system("rm file");
}

// Tests writing new file.
void test_file_write()
{
    struct file_output fo = f_exec(FILE_WRITE, "file#Hello, World");
    char *file_str = malloc(20);
    FILE *f = fopen("file", "r");

    fscanf(f, "A-Z a-z,", file_str);
    assert(strcmp(file_str, "Hello, World") == 0);
    fclose(f);
    free(file_str);
    system("rm file");
}

// Tests appending to a file.
void test_file_append()
{
    struct file_output fo = f_exec(FILE_APPEND, "file#Hello, World");
    char *file_str = malloc(13);
    FILE *f = fopen("file", "r");

    fscanf(f, "A-Z a-z,", file_str);
    assert(strcmp(file_str, "Hello, World") == 0);
    fclose(f);
    free(file_str);
    system("rm file");
}
