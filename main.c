#include "client.h"
#include "network.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
/*void run_chat_loop(int secure_pipe) {
  char input_buffer[PAYLOAD_SIZE];
  struct DataPacket incoming_packet;
  int result;
  printf(" \n ------------------  Ahlan beekom fel bo7eera ------------------- "
         "\n");

  FILE *output_file = NULL; // Initialize output_file to NULL

  int files_sent = 0;
  while (1) {
    // read_fds is an fd_set, which is a set of file descriptors that will now
    // be monitored (polled?) by select or poll until one file descriptor is
    // ready for I/O operations
    //
    fd_set read_fds;
    // clears all the file descriptors in read_fds
    FD_ZERO(&read_fds);

    // keyboard input for windows
    // puts 0 (file descriptor), which is stdin into read_fds
    // does not work on windows, as sockets arent file descriptors. on windows,
    // only sockets should be put into the fd_set, as select only accepts
    // SOCKETS
    FD_SET(0, &read_fds);
    // watching network socket puts the secure
    // pipe socket into read_fds. this is a macro, so different types fine, and
    // SOCKET is pretty much? an int
    FD_SET(secure_pipe, &read_fds);

    // starts monitoring
    // how much select checks. neede so it doesn't block
    struct timeval tv = {.tv_sec = 0, .tv_usec = 100000};
    result = select(secure_pipe + 1, &read_fds, NULL, NULL, &tv);
    if (result < 0) {
      printf("ERROR: select() failed with error code %d\n", errno);
      break;
void run_chat_loop(SOCKET secure_pipe){
    char input_buffer[PAYLOAD_SIZE];
    DataPacket incoming_packet;
    int result;
    printf(" \n ------------------  Ahlan beekom fel bo7eera ------------------- \n");

FILE* output_file = NULL; // Initialize output_file to NULL

    while(1){
        fd_set read_fds;
        FD_ZERO(&read_fds);

        //keyboard input for windows
        FD_SET(0, &read_fds);
        //watching network socket
        FD_SET(secure_pipe, &read_fds);
        
        result = select(0, &read_fds, NULL, NULL, NULL);

        if (result == SOCKET_ERROR) {
            printf("ERROR: select() failed with error code %d\n", WSAGetLastError());
            break;
        }
        // -------------------------
        //KEYBOARD INPUT
        //------------------------
        if(FD_ISSET(0, &read_fds)){
            if(fgets(input_buffer, sizeof(input_buffer), stdin) != NULL){
                
                if (strncmp(input_buffer, "/file", 6) == 0){
                    //extract the file path from the input command
                    char filepath[256];
                    sscanf(input_buffer, "/file %s", filepath);
                    printf("[FILE TRANSFER INITIATED]: Attempting to Stream file: %s\n", filepath);
                    //open the file for reading in binary mode
                    FILE* source_file = fopen(filepath, "rb");
                    if (source_file == NULL) {
                        printf("ERROR: Could not open file %s for reading, check u wrote correct path.\n", filepath);
                        continue;
                    }
                    DataPacket file_packet;
                    size_t bytes_read;
                    unsigned long total_bytes_sent = 0;
                    //read the file in chunks and send each chunk as a DataPacket
                    while ((bytes_read = fread(file_packet.payload,1, PAYLOAD_SIZE, source_file)) > 0 ){
                        file_packet.type = Packet_file_chunk;
                        file_packet.length = (uint16_t)bytes_read;
                        
                        send(secure_pipe, (char*)&file_packet, sizeof(DataPacket), 0);
                        total_bytes_sent += bytes_read;

                        // el eof marker
                        file_packet.type = Packet_file_eof;
                        file_packet.length = 0;
                        send(secure_pipe, (char*)&file_packet, sizeof(DataPacket), 0);

                        fclose(source_file);
                        printf("[FILE TRANSFER COMPLETE]: Total bytes sent: %lu\n", total_bytes_sent);
                    }
                }
                else{
                // STANDARD CHAT MESSAGE
                DataPacket send_packet;
                send_packet.type = PacketChat;

                strncopy(send_packet.payload, input_buffer, PAYLOAD_SIZE);
                send_packet.payload [PAYLOAD_SIZE - 1] = '\0'; // Ensure null-termination
                send_packet.length = (uint16_t)strlen(send_packet.payload);

                int bytes_sent = send(secure_pipe, (char*)&send_packet, sizeof(DataPacket), 0); // send everything 
                
                if (bytes_sent == SOCKET_ERROR) {
                    printf("ERROR: send() failed with error code %d\n", WSAGetLastError());
                    break;
                }
            }
            }
        }
        if(FD_ISSET(secure_pipe, &read_fds)){
            int bytes_recieved = recv(secure_pipe, (char*)&incoming_packet, sizeof(DataPacket), 0);
            if (bytes_recieved <= 0) {
                printf("\n[DISCONNECTED] SOMEONE HAS LEFT THE CHAT OR CONNCECTION LOST.\n");
                break;
            }
            // Handle incoming packet based on its type
            if (incoming_packet.type == PacketChat) {
                incoming_packet.payload[PAYLOAD_SIZE - 1] = '\0'; // Ensure null-termination
                
                printf("\n[INCOMING MESSAGE]: %s\n", incoming_packet.payload);
            }
            else if(incoming_packet.type == Packet_file_chunk){
                if (output_file == NULL) {
                    printf(" [FILE TRANSFER INITIATED]: Receiving file...\n");
                    output_file = fopen("received_file.dat", "wb");
                }
                if (output_file != NULL) {
                    fwrite(incoming_packet.payload, 1, incoming_packet.length, output_file);
                }
            }
            else if(incoming_packet.type == Packet_file_eof){
                if (output_file != NULL) {
                    fclose(output_file);
                    output_file = NULL;
                    printf("[FILE TRANSFER COMPLETE]: File received successfully.\n");
                }
            }
        }
    }
}
// ----------------------------------
//MAIN ENTRY AND DRIVER HANDSHAKE 
// ----------------------------------
int main(int argc, char *argv[]) { //da for intilization meen host weh meen client
    WSADATA wsaData; 
    int wsa_check = WSAStartup(MAKEWORD(2, 2), &wsaData); // to start using network 
    if (wsa_check != 0) {
        printf("FY MASHAKEL BOS 3ALEHA: %d\n", wsa_check);
        return 1;
    }
    // -------------------------
    // KEYBOARD INPUT
    //------------------------
    if (FD_ISSET(0, &read_fds)) {
      if (fgets(input_buffer, sizeof(input_buffer), stdin) != NULL) {

        if (strncmp(input_buffer, "/file", 5) == 0) {
          // extract the file path from the input command
          char filepath[256];
          sscanf(input_buffer, "/file %s", filepath);
          printf("[FILE TRANSFER INITIATED]: Attempting to Stream file: %s\n",
                 filepath);
          // open the file for reading in binary mode
          FILE *source_file = fopen(filepath, "rb");
          if (source_file == NULL) {
            printf("ERROR: Could not open file %s for reading, check u wrote "
                   "correct path.\n",
                   filepath);
            continue;
          }
          struct DataPacket file_packet;
          size_t bytes_read;
          unsigned long total_bytes_sent = 0;
          // read the file in chunks and send each chunk as a DataPacket
          while ((bytes_read = fread(file_packet.payload, 1, PAYLOAD_SIZE,
                                     source_file)) > 0) {
            file_packet.type = Packet_file_chunk;
            file_packet.length = (uint16_t)bytes_read;
            total_bytes_sent += bytes_read;
          }
          send(secure_pipe, (char *)&file_packet, sizeof(struct DataPacket), 0);

          // el eof marker
          file_packet.type = Packet_file_eof;
          file_packet.length = 0;
          send(secure_pipe, (char *)&file_packet, sizeof(struct DataPacket), 0);

          fclose(source_file);
          printf("[FILE TRANSFER COMPLETE]: Total bytes sent: %lu\n",
                 total_bytes_sent);
        } else {
          // STANDARD CHAT MESSAGE
          struct DataPacket send_packet;
          send_packet.type = PacketChat;

          strncpy(send_packet.payload, input_buffer, PAYLOAD_SIZE);
          // why. is it not done automatically?
          // no. if the message is more than 1024(PAYLOAD_SIZE) characters no.
          send_packet.payload[PAYLOAD_SIZE - 1] =
              '\0'; // Ensure null-termination
          send_packet.length = (uint16_t)strlen(send_packet.payload);

          // sends through the socket a casted pointer to string of send packet
          int bytes_sent =
              send(secure_pipe, (char *)&send_packet, sizeof(struct DataPacket),
                   0); // send everything

          if (bytes_sent == -1) {
            printf("ERROR: send() failed with error code %d\n", errno);
            break;
          }
        }
      }
    }
    if (FD_ISSET(secure_pipe, &read_fds)) {
      int bytes_recieved = recv(secure_pipe, (char *)&incoming_packet,
                                sizeof(struct DataPacket), 0);
      if (bytes_recieved <= 0) {
        printf("\n[DISCONNECTED] SOMEONE HAS LEFT THE CHAT OR CONNCECTION "
               "LOST.\n");
        break;
      }
      // Handle incoming packet based on its type
      if (incoming_packet.type == PacketChat) {
        incoming_packet.payload[PAYLOAD_SIZE - 1] =
            '\0'; // Ensure null-termination

        printf("\n[INCOMING MESSAGE]: %s\n", incoming_packet.payload);
      } else if (incoming_packet.type == Packet_file_chunk) {
        if (output_file == NULL) {
          printf(" [FILE TRANSFER INITIATED]: Receiving file...\n");
          // just to figure out size of s;
          int digits;
          int temp = files_sent;
          while (temp > 0) {
            digits++;
            temp = temp / 10;
          }
          // char s[18 + digits];
          char u[14 + digits];
          sprintf(u, "%s%d", "recieved_file", files_sent + 1);
          char *s = strcat(u, ".dat");
          output_file = fopen(s, "wb");
        }
        if (output_file != NULL) {
          fwrite(incoming_packet.payload, 1, incoming_packet.length,
                 output_file);
        }
      } else if (incoming_packet.type == Packet_file_eof) {
        if (output_file != NULL) {
          fclose(output_file);
          output_file = NULL;
          printf("[FILE TRANSFER COMPLETE]: File received successfully.\n");
          files_sent++;
        }
      }
    }
  }
}*/
// ----------------------------------
// MAIN ENTRY AND DRIVER HANDSHAKE
// ----------------------------------
int main(int argc,
         char *argv[]) { // da for intilization meen host weh meen client

  int final_pipe = -1;
  if (argc > 1 && strcmp(argv[1], "host") == 0) {
    printf("attempting to start host\n");
    final_pipe = start_host();
  } else if (argc > 2 && strcmp(argv[1], "client") == 0) {

    final_pipe = start_guest(argv[2]);

  } else {
    printf("ERROR: Please specify 'host' or 'client <IP_ADDRESS>' as command "
           "line arguments.\n");
    // c  wasn't capital
    return 1;
  }

  if (final_pipe == -1) {
    printf("ERROR: Failed to establish a connection.\n");
    return 1;
  }
  client_loop(final_pipe);
  return 0;
}
