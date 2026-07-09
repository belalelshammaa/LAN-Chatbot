#include "protocol.h"

void run_chat_loop(SOCKET secure_pipe);

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
