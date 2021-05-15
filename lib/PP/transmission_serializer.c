#include <PP/transmission.h>
#include <string.h>
#include <stdlib.h>

const unsigned SERIALIZED_SIZE_WITHOUT_DATA = sizeof(transmission) - (sizeof(void *) + sizeof(char *));

// Serializes transmission struct.
// Error message in transmission and connection is ignored.
void *transmission_serialize(transmission t)
{
    void *serialization = malloc(SERIALIZED_SIZE_WITHOUT_DATA + t.header.bytes);
    memcpy(serialization, &t.header.bytes, sizeof(t.header.bytes));
    memcpy(serialization + sizeof(t.header.bytes), &t.header.chunk_size, sizeof(t.header.chunk_size));
    memcpy(serialization + sizeof(t.header.bytes) + sizeof(t.header.chunk_size), &t.open, sizeof(t.open));
    memcpy(serialization + sizeof(t.header.bytes) + sizeof(t.header.chunk_size) + sizeof(t.open),
            &t.error, sizeof(t.error));
    memcpy(serialization + sizeof(t.header.bytes) + sizeof(t.header.chunk_size) + sizeof(t.open) + sizeof(t.error),
            &t.error_len, sizeof(t.error_len));
    memcpy(serialization + sizeof(t.header.bytes) + sizeof(t.header.chunk_size) + sizeof(t.open) +
            sizeof(t.error) + sizeof(t.error_len), t.data, t.header.bytes);
    memcpy(serialization + sizeof(t.header.bytes) + sizeof(t.header.chunk_size) + sizeof(t.open) +
            sizeof(t.error) + sizeof(t.error_len) + t.header.bytes, &t.connection.error,
            sizeof(t.connection.error));
    memcpy(serialization + sizeof(t.header.bytes) + sizeof(t.header.chunk_size) + sizeof(t.open) +
            sizeof(t.error) + sizeof(t.error_len) + t.header.bytes + sizeof(t.connection.error),
            &t.connection.fd, sizeof(t.connection.fd));
    memcpy(serialization + sizeof(t.header.bytes) + sizeof(t.header.chunk_size) + sizeof(t.open) +
            sizeof(t.error) + sizeof(t.error_len) + t.header.bytes + sizeof(t.connection.error) +
            sizeof(t.connection.fd), &t.connection.max_bytes, sizeof(t.connection.max_bytes));

    return serialization;
}

// Deserialized transmission struct.
transmission transmission_deserialize(const void *serialization)
{
    transmission t;
    memcpy(&t.header.bytes, serialization, sizeof(t.header.bytes));
    memcpy(&t.header.chunk_size, serialization + sizeof(t.header.bytes), sizeof(t.header.chunk_size));
    memcpy(&t.open, serialization  + sizeof(t.header.bytes) + sizeof(t.header.chunk_size), sizeof(t.open));
    memcpy(&t.error, serialization  + sizeof(t.header.bytes) + sizeof(t.header.chunk_size) + sizeof(t.open),
            sizeof(t.error));
    memcpy(&t.error_len, serialization  + sizeof(t.header.bytes) + sizeof(t.header.chunk_size) + sizeof(t.open) +
            sizeof(t.error), sizeof(t.error_len));

    t.data = malloc(t.header.bytes);
    memcpy(t.data, serialization + sizeof(t.header.bytes) + sizeof(t.header.chunk_size) + sizeof(t.open) +
            sizeof(t.error) + sizeof(t.error_len), t.header.bytes);
    memcpy(&t.connection.error, serialization + sizeof(t.header.bytes) + sizeof(t.header.chunk_size) +
            sizeof(t.open) + sizeof(t.error) + sizeof(t.error_len) + t.header.bytes,
            sizeof(t.connection.error));
    memcpy(&t.connection.fd, serialization + sizeof(t.header.bytes) + sizeof(t.header.chunk_size) + sizeof(t.open) +
            sizeof(t.error) + sizeof(t.error_len) + t.header.bytes + sizeof(t.connection.error),
            sizeof(t.connection.fd));
    memcpy(&t.connection.max_bytes, serialization + sizeof(t.header.bytes) + sizeof(t.header.chunk_size) +
            sizeof(t.open) + sizeof(t.error) + sizeof(t.error_len) + t.header.bytes +
            sizeof(t.connection.error) + sizeof(t.connection.fd), sizeof(t.connection.max_bytes));

    return t;
}
