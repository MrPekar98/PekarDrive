#include <PP/pp.h>
#include <stdlib.h>
#include <string.h>
#include <PP/transmission.h>

// Makes connection from hostname and port number.
pp_endpoint endpoint_from_address(const char *hostname, unsigned short port)
{
    conn connection = client_init(hostname, port);
    return (pp_endpoint) {.connection = connection};
}

// Makes connection from another established connection.
pp_endpoint endpoint_from_connection(conn connection)
{
    return (pp_endpoint) {.connection = connection};
}

// Sends message.
pp_status pp_send(pp_endpoint endpoint, const void *data, size_t len)
{
    transmission message = init_transmission(endpoint.connection, data, len);
    pp_status status = {.error = 0, .error_message = NULL, .bytes = 0};

    if (message.error || !transmit(&message))
    {
        const char *error_msg = transmission_error(message);
        status.error_message = malloc(strlen(error_msg));
        status.bytes = strlen(error_msg);
        status.error = 1;
        strcpy(status.error_message, error_msg);
        close_transmission(&message);
    }

    close_transmission(&message);
    return (pp_status) {.error = 0, .bytes = 0, .error_message = NULL};
}

// Reads message.
pp_message pp_read(pp_endpoint endpoint)
{
    transmission t = init_transmission(endpoint.connection, NULL, 0);

    if (!receive(&t))
    {
        pp_status status = {.error = 1, .error_message = NULL, .bytes = 0};

        if (t.error)
        {
            const char *error_msg = transmission_error(t);
            status.error_message = malloc(strlen(error_msg));
            strcpy(status.error_message, error_msg);
            status.bytes = strlen(error_msg);
        }

        return (pp_message) {.status = status, .buffer = NULL};
    }

    pp_message message = {.buffer = malloc(transmission_size(t))};
    memcpy(message.buffer, transmission_data(t), transmission_size(t));
    message.status = (pp_status) {.error = 0, .error_message = NULL, .bytes = 0};

    return message;
}
