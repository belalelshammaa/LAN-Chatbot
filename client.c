#include "client.h"
#include "network.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
FILE *output_file = NULL; // Initialize output_file to NULL
int handle_message(int socket, char *input) {
  if (input == NULL) {
    printf("null input\n");
    return 0;
  }
  struct DataPacket send_packet;
  send_packet.type = PCKT_CHAT;

  strncpy(send_packet.payload, input, PAYLOAD_SIZE);
  // why. is it not done automatically?
  // no. if the message is more than 1024(PAYLOAD_SIZE) characters no.
  send_packet.payload[PAYLOAD_SIZE - 1] = '\0'; // Ensure null-termination
  send_packet.length = (uint16_t)strlen(send_packet.payload);

  // sends through the socket a casted pointer to string of send packet
  long int bytes_sent =
      send(socket, (char *)&send_packet, sizeof(struct DataPacket),
           0); // send everything

  if (bytes_sent == -1) {
    printf("ERROR: send() failed with error code %d\n", errno);
    return -1;
  }
  return 0;
}
int handle_file(int socket, char *filepath) {
  if (filepath == NULL) {
    printf("empty file");
    return 0;
  }
  printf("[FILE TRANSFER INITIATED]: Attempting to Stream file: %s\n",
         filepath);
  // open the file for reading in binary mode
  FILE *source_file = fopen(filepath, "rb");
  if (source_file == NULL) {
    printf("ERROR: Could not open file %s for reading, check u wrote "
           "correct path.\n",
           filepath);
    return 0;
  }
  struct DataPacket file_packet_start;
  strcpy(file_packet_start.payload, filepath);
  // enzure null-termination
  file_packet_start.payload[strlen(filepath)] = '\0';
  file_packet_start.length = (uint16_t)strlen(file_packet_start.payload);
  file_packet_start.type = PCKT_FILE_START;

  if (send(socket, (char *)&file_packet_start, sizeof(struct DataPacket), 0) ==
      -1) {
    printf("couldn't send file header to server");
    return -1;
  }
  struct DataPacket file_packet;
  size_t bytes_read;
  unsigned long total_bytes_sent = 0;
  // read the file in chunks and send each chunk as a DataPacket
  while ((bytes_read =
              fread(file_packet.payload, 1, PAYLOAD_SIZE, source_file)) > 0) {
    file_packet.type = PCKT_FILE_CHUNK;
    file_packet.length = (uint16_t)bytes_read;
    total_bytes_sent += bytes_read;
    send(socket, (char *)&file_packet, sizeof(struct DataPacket), 0);
  }

  // el eof marker
  file_packet.type = PCKT_FILE_END;
  file_packet.length = 0;
  send(socket, (char *)&file_packet, sizeof(struct DataPacket), 0);

  fclose(source_file);
  printf("[FILE TRANSFER COMPLETE]: Total bytes sent: %lu\n", total_bytes_sent);
  return 0;
}
int handle_exit(int socket, char *_input) {
  struct DataPacket send_packet = {
      .length = 0,
      .payload = {0},
      .type = PCKT_LEAVE,
  };
  if (send(socket, (char *)&send_packet, sizeof(struct DataPacket), 0) == -1) {
    printf("could not send leave packet from client to server\n");
    return -1;
  }
  if (close(socket) == -1) {
    return -1;
  }
  return 1;
}
struct command_t {
  const char *name;
  int (*handler)(int, char *);
};
static const struct command_t command_handler[] = {
    {"/msg", handle_message},
    {"/file", handle_file},
    {"/exit", handle_exit},
};
int packet_message(struct DataPacket *packet) {

  packet->payload[PAYLOAD_SIZE - 1] = '\0'; // Ensure null-termination

  printf("\n[INCOMING MESSAGE]: %s\n", packet->payload);
  return 0;
}
int packet_join(struct DataPacket *packet) {
  packet->payload[PAYLOAD_SIZE - 1] = '\0'; // Ensure null-termination

  printf("\n: %s has joined\n", packet->payload);
  return 0;
}
int packet_leave(struct DataPacket *packet) {
  packet->payload[PAYLOAD_SIZE - 1] = '\0'; // Ensure null-termination

  printf("\n: %s has left\n", packet->payload);
  return 0;
}
int packet_start(struct DataPacket *packet) {
  packet->payload[PAYLOAD_SIZE - 1] = '\0'; // Ensure null-termination
  char filename[256] = "../files/";
  strcat(filename, packet->payload);
  output_file = fopen(filename, "wb");
  printf("created file %s\n", filename);
  return 0;
}
int packet_end(struct DataPacket *_packet) {
  if (output_file != NULL) {
    fclose(output_file);
    output_file = NULL;
  }
  printf("[FILE TRANSFER COMPLETE]: File received successfully.\n");
  return 0;
}
int packet_chunk(struct DataPacket *packet) {

  if (output_file != NULL) {
    fwrite(packet->payload, 1, packet->length, output_file);
    return 0;
  }
  return -1;
}
static int (*packet_handler[])(struct DataPacket *) = {
    packet_join,  packet_leave, packet_message,
    packet_start, packet_chunk, packet_end,
};
int client_loop(int socket) {

  char input_buffer[PAYLOAD_SIZE];
  struct DataPacket incoming_packet;
  int result;
  printf("Welcome!\n");

  while (1) {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(0, &read_fds);
    FD_SET(socket, &read_fds);

    struct timeval tv = {.tv_sec = 0, .tv_usec = 100000};
    result = select(socket + 1, &read_fds, NULL, NULL, &tv);

    if (result < 0) {
      printf("ERROR: select() failed with error code %d\n", errno);
      break;
    }

    if (FD_ISSET(0, &read_fds)) {
      // split using spaces
      int flag = 1;
      if (fgets(input_buffer, sizeof(input_buffer), stdin) != NULL) {
        char *ptr = strchr(input_buffer, '\n');
        if (ptr != NULL) {
          *ptr = '\0';
        }
        char temp[sizeof input_buffer];
        strcpy(temp, input_buffer);
        char *cmd = strtok(temp, " ");
        char *args = strtok(NULL, "");
        if (cmd != NULL) {
          for (size_t i = 0;
               i < (sizeof command_handler / sizeof(struct command_t)); i++) {
            if (strcmp(cmd, command_handler[i].name) == 0) {
              flag = 0;
              int command_result;
              if ((command_result = command_handler[i].handler(socket, args)) ==
                  -1) {
                return -1;
              } else if (command_result == 1) {
                return 0;
              } else {
                break;
              }
            }
          }
          if (flag) {
            if (command_handler[0].handler(socket, input_buffer) == -1) {
              return -1;
            };
          }
        }
      } else {
        return -1;
      }
    }
    if (FD_ISSET(socket, &read_fds)) {
      int bytes_recieved =
          recv(socket, (char *)&incoming_packet, sizeof(struct DataPacket), 0);
      if (bytes_recieved <= 0) {
        printf("\n[DISCONNECTED] Server has disconnected "
               "\n");
        return -1;
      }
      // Handle incoming packet based on its type
      if (packet_handler[incoming_packet.type](&incoming_packet) == -1) {
        return -1;
      };
    }
  }
  return result;
}
