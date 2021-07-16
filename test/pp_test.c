#include <PP/pp.h>
#include <assert.h>
#include "pp_test_server.h"

void test_send_and_read();

int main()
{
    pid_t pid = pp_start_server();
    sleep(5);
    test_send_and_read();
    pp_stop_server(pid);

    return 0;
}

void test_send_and_read()
{
    pp_endpoint server = endpoint_from_address("127.0.0.1", pp_test_server_port());
    assert(!pp_send(server, "Test", 4).error);
    sleep(1);

    pp_message message = pp_read(server);
    assert(!message.status.error);
    assert(((char *) message.buffer)[0] == 'S');
}
