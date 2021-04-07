#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <PP/transmission.h>

#define DEFAULT_TRANS init_transmission((conn) {.error = 0, .fd = 1, .max_bytes = 1}, "Test", 4)

void test_construction();
void test_chunk_size_setter();
void test_transmission_data_size();
void test_get_transmission_data();
void test_serialization();

int main()
{
    test_construction();
    test_chunk_size_setter();
    test_transmission_data_size();
    test_get_transmission_data();
    test_serialization();

    return 0;
}

// Test construction.
void test_construction()
{
    transmission t = DEFAULT_TRANS;
    assert(strcmp((char *) t.data, "Test") == 0);
    assert(t.open);
    assert(!t.error);
    assert(t.error_len == 0);
    assert(t.header.bytes == 4);
    assert(t.header.chunk_size == get_transmission_chunk_size());
    assert(t.connection.fd == 1);
    assert(!t.connection.error);
    assert(t.connection.max_bytes == 1);

    close_transmission(&t);
}

// Test chunk size setter.
void test_chunk_size_setter()
{
    set_transmission_chunk_size(1);
    assert(get_transmission_chunk_size() == 1);
    set_transmission_chunk_size(DEFAULT_CHUNK_SIZE);
}

// Test size of transmission data.
void test_transmission_data_size()
{
    transmission t = DEFAULT_TRANS;
    assert(transmission_size(t) == 4);
    close_transmission(&t);
}

// Test getter of transmission data.
void test_get_transmission_data()
{
    transmission t = DEFAULT_TRANS;
    assert(strcmp((char *) transmission_data(t), "Test") == 0);
    close_transmission(&t);
}

// Test serialization.
void test_serialization()
{
    transmission t = DEFAULT_TRANS;
    void *serial = transmission_serialize(t);
    transmission de_serial = transmission_deserialize(serial);
    free(serial);

    assert(de_serial.open);
    assert(!de_serial.error);
    assert(de_serial.error_len == t.error_len);
    assert(de_serial.header.bytes == t.header.bytes);
    assert(de_serial.header.chunk_size == get_transmission_chunk_size());
    assert(de_serial.connection.fd == t.connection.fd);
    assert(!de_serial.connection.error);
    assert(de_serial.connection.max_bytes == t.connection.max_bytes);
    assert(strcmp((char *) t.data, (char *) de_serial.data) == 0);

    close_transmission(&t);
}
