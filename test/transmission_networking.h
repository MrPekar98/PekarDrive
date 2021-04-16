#include <unistd.h>

pid_t setup_server(int (*assert)(void *));
void stop_server(pid_t pid);
unsigned short get_test_server_port();
