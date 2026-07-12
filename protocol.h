#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAYLOAD_SIZE 1024
// functions like getaddrinfo require this to be a string and not an int
#define DEFAULT_PORT "8888"

enum PacketType {
  PacketChat = 1,
  Packet_file_chunk = 2,
  Packet_file_eof = 3,
};

typedef struct {
  uint8_t type;
  uint16_t length;
  char payload[PAYLOAD_SIZE];
} DataPacket;

#endif
