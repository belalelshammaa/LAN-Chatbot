#ifndef NETWORK_H
#define NETWORK_H
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <stdint.h>

#define PAYLOAD_SIZE 1024
#define MAX_CLIENTS 64
// functions like getaddrinfo require this to be a string and not an int
#define DEFAULT_PORT "8886"

enum PacketType {
  PCKT_JOIN = 0,
  PCKT_LEAVE = 1,
  PCKT_CHAT = 2,
  PCKT_FILE_START = 3,
  PCKT_FILE_CHUNK = 4,
  PCKT_FILE_END = 5,
};

typedef char uuid[32];
struct DataPacket {
  char payload[PAYLOAD_SIZE];
  uuid id;
  uint16_t length;
  uint8_t type;
};

int start_client(char *server_address);
#endif
