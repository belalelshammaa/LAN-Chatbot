#include "protocol.h"

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
    printf("EL NETWORK FEL SALEEM WEH ZAY EL FOL.\n\n");

    SOCKET final_pipe = INVALID_SOCKET;
    if(argc>1 && strcmp(argv[1], "host") == 0) {
        final_pipe = start_host();
    } else if(argc>2 && strcmp(argv[1], "client") == 0) {
        final_pipe = start_guest(argv[2]);

    } else {
        printf("ERROR: Please specify 'host' or 'client <IP_ADDRESS>' as command line arguments.\n");
    WSAcleanup();
        return 1;
    }
    
    if (final_pipe == INVALID_SOCKET) {
        printf("ERROR: Failed to establish a connection.\n");
        WSAcleanup();
        return 1;
    } 
    run_chat_loop(final_pipe);
    return 0;   
}
