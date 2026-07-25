#ifndef CLIENT_H
#define CLIENT_H
#include "network.h"
#define NAME_LENGTH 64
struct Client {
  char name[NAME_LENGTH];
  uuid uuid;
  int socket;
};
int client_loop(int);
#endif
