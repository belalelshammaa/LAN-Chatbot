#include "network.h"
#include "client.h"
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
char client_name[256];
int start_client(char *server_address) {
  struct addrinfo *result, *ptr, hints;
  memset(&hints, 0, sizeof hints);
  // could cause program to fail. if it does, try setting it to as specific type
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  // hints.ai_flags = AI_PASSIVE;
  hints.ai_protocol = IPPROTO_TCP;
  int addr_result;
  if ((addr_result =
           getaddrinfo(server_address, DEFAULT_PORT, &hints, &result)) != 0) {
    // stdout or stderr
    printf("error in getaddrinfo: %d\n", addr_result);
    return -1;
  }
  int sock = -1;
  struct DataPacket send_packet;
  send_packet.type = PCKT_JOIN;
  printf("enter your name please\n");
  scanf("%s", client_name);
  strcpy(send_packet.payload, client_name);
  send_packet.length = (uint16_t)strlen(send_packet.payload);
  for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
    if ((sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol)) ==
        -1) {
      printf("could not create socket\n");
      freeaddrinfo(result);
      return -1;
    }
    if (connect(sock, ptr->ai_addr, ptr->ai_addrlen) == -1) {
      printf("attempting other address in linked list\n");
      close(sock);
      sock = -1;
      continue;
    };
    break;
  }
  freeaddrinfo(result);
  printf("connection established. sd number: %d\n", sock);
  send(sock, (char *)&send_packet, sizeof(struct DataPacket), 0);
  return sock;
}
// sockaddr is general so stuff like bind() don't have to worry about family
// type ipv4 or 6
//
// sockaddr_in is ipv4, contains field for port, ip address,
// padding so casting between it and sockaddr also works, and family type, whicm
// is always AF_INET
//
// sockaddr_in6 is ipv6, slightly different field, but don't worry
//
// in_addr is ipv4 address, which sockaddr_in contains
// same for in6_addr
//
// getaddrinfo() returns linked list addrinfo which is used to get info about
// socket connection and creation. first thing called usually
