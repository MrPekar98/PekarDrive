#include <assert.h>
#include "../src/fs/fs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Prototypes.
void test_creation();
void test_deletion();
void test_writing();
void test_reading();

int main()
{
    test_creation();
    test_deletion();
    test_writing();
    test_reading();

    return 0;
}

// Test creation of file.
void test_creation()
{
    assert(fs_create_file("test.txt"));
    system("rm test.txt");
}

// Test deletion of file.
void test_deletion()
{
    system("touch test.txt");
    fs_delete_file("test.txt");

    assert(fs_write_file("test.txt", NULL, 0, 1) == -1);
}

// Test writing to file.
void test_writing()
{
    void *buffer = malloc(2);
    memcpy(buffer, "ab", 2);

    fs_create_file("test.txt");
    assert(fs_write_file("test.txt", buffer, 2, 0) == 2);
    free(buffer);
    system("rm test.txt");
}

// Test reading of file.
void test_reading()
{
    system("touch test.txt");
    fs_write_file("test.txt", "abc", 3, 0);

    void *buffer = fs_read_file("test.txt");
    assert(strcmp("abc", (char *) buffer) == 0);

    free(buffer);
    system("rm test.txt");
}
