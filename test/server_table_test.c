#include <assert.h>
#include <stddef.h>
#include "../src/server_table.h"

static inline void add_servers();
void test_add_servers();
void test_remove_servers();
void test_get_id();
void test_get_index();
void test_get_predicate();
short pred(struct file_server server);

int main()
{
    add_servers();

    test_add_servers();
    test_remove_servers();
    test_get_id();
    test_get_index();
    test_get_predicate();

    return 0;
}

// Adds servers. Can be used before each test function.
// Adds 3 servers with IDs 1, 2 and 3.
static inline void add_servers()
{
    struct file_server t1 = {.id = 1, .size = 1, .location = {.ip = NULL, .port = 0}},
                       t2 = {.id = 2, .size = 2, .location = {.ip = NULL, .port = 0}},
                       t3 = {.id = 3, .size = 3, .location = {.ip = NULL, .port = 0}};

    add_server(t1);
    add_server(t2);
    add_server(t3);
}

// Tests adding servers.
void test_add_servers()
{
    assert(table_count() == 3);
}

// Tests removing servers.
void test_remove_servers()
{
    assert(table_count() == 3);

    remove_server(1);
    assert(table_count() == 2);

    remove_server(3);
    assert(table_count() == 1);

    remove_server(2);
    assert(table_count() == 0);
    add_servers();
}

// Tests getting server by id.
void test_get_id()
{
    assert(getfs_id(2) != NULL);
    assert(getfs_id(5) == NULL);
}

// Tests getting server by index.
void test_get_index()
{
    assert(getfs_index(0) != NULL);
    assert(getfs_index(2) != NULL);
    assert(getfs_index(3) == NULL);
}

// Tests getting server by predicate function.
void test_get_predicate()
{
    assert(getfs_pred(pred) != NULL);
}

// Predicate function.
short pred(struct file_server server)
{
    return server.id == 2;
}
