#include "protocol.h"
#include <stdio.h>
#include <unistd.h>
// #include <psdk_inc/_socket_types.h>
//  #include <stdio.h>
#include <string.h>
// #include <winsock2.h>
// #include <ws2tcpip.h>
int start_host(void) {
  struct addrinfo *result = NULL, hints;
  // makesure hints is 0, no data in it so the result we get is how we want
  memset(&hints, 0, sizeof hints);
  // could cause program to fail. if it does, try setting it to as specific type
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  // fill in my ip for me, not manual;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;
  // null parameter was for manual ip address insertion
  int addr_result;
  if ((addr_result = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result)) != 0) {
    // stdout or stderr
    printf("error in getaddrinfo: %d\n", addr_result);
    return -1;
  }
  int sock = -1;

  printf("socktype: %d,family: %d, protocol: %d\n", result->ai_socktype,
         result->ai_family, result->ai_protocol);
  sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

  // bind associates socket with specific port number
  // only done on server/host as for the client they will attempt to connect to
  // a port and not listen
  int bind_result;
  if ((bind_result = bind(sock, result->ai_addr, result->ai_addrlen)) != 0) {
    printf("error in binding socket to port: %d\n", bind_result);
    return -1;
  };
  // listening
  int listen_result;
  if ((listen_result = listen(sock, SOMAXCONN) != 0)) {
    printf("could not listen : %d\n", listen_result);
    return -1;
  };
  return sock;
}

int start_guest(char *server_address) {
  struct addrinfo *result, *ptr, hints;
  memset(&hints, 0, sizeof hints);
  // could cause program to fail. if it does, try setting it to as specific type
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_protocol = IPPROTO_TCP;
  int addr_result;
  if ((addr_result =
           getaddrinfo(server_address, DEFAULT_PORT, &hints, &result)) != 0) {
    // stdout or stderr
    printf("error in getaddrinfo: %d\n", addr_result);
    return -1;
  }
  int sock = -1;
  for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
    if ((sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol)) ==
        -1) {
      printf("could not create socket\n");
      return -1;
    }
    int connect_result;
    if ((connect_result = connect(sock, ptr->ai_addr, ptr->ai_addrlen)) == -1) {
      close(sock);
      sock = -1;
      continue;
    };
    break;
  }
  freeaddrinfo(result);
  printf("connection established\n");
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
