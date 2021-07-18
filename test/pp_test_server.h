#ifndef PP_TEST_SERVER_H
#define PP_TEST_SERVER_H

#include <unistd.h>

pid_t pp_start_server();
void pp_stop_server(pid_t pid);
unsigned short pp_test_server_port();

#endif
