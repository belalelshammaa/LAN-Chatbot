#ifndef NETWORK_H
#define NETWORK_H
#include "protocol.h";
SOCKET start_host(void);
SOCKET start_guest(char *server_address);
#endif
