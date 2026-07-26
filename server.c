#include "network.h"
#include "client.h"
#include "vec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netdb.h>

int main(void) {
    printf("Starting the central server on port %s ...\n", DEFAULT_PORT);

    // 1. prepare the config "hints" for getaddrinfo
    struct addrinfo hints, *results;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // support both IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM; // tcp stream sockets
    hints.ai_flags = AI_PASSIVE; // tell the os to fill in my ip for me

    // 2. resolve the address and port
    int addr_status = getaddrinfo(NULL, DEFAULT_PORT, &hints, &results);
    if (addr_status != 0) {
        printf("getaddrinfo failed: %d\n", addr_status);
        return 1;
    }

    // 3. ask the os for a socket
    int server_socket = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    if (server_socket == -1) {
        printf("socket creation failed\n");
        freeaddrinfo(results);
        return 1;
    }
    printf("Server socket created successfully\n");
    // REMOVED: return 0; (This was killing the server instantly!)

    // 4. socket options
    int opt = 1;
    if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        printf(" [WARNING] : FAILED TO SET SO_REUSEADDR. \n ");
    }

    // 5. bind the socket to our specific port
    if(bind(server_socket, results->ai_addr, results->ai_addrlen) == -1){
        printf("ERROR: could not bind the socket to port %s. \n", DEFAULT_PORT);
        freeaddrinfo(results);
        return 1;
    }

    // 6. tell the OS to start listening for clients that want to connect
    if (listen(server_socket, MAX_CLIENTS) == -1){
        printf("ERROR: failed to start listening. \n");
        freeaddrinfo(results);
        return 1;
    }

    printf("EL SERVER ZAY EL FOL WEH LISTENING 3ALA PORT %s AND WAITING FOR CLIENTS... \n", DEFAULT_PORT);

    freeaddrinfo(results); // Safely free this now that we are bound and listening

    // 7. initialize our dynamic client list
    struct Vec clients;
    if(vec_new(&clients, sizeof(struct Client)) != 0){
        printf(" ERROR: FAILED TO INITIALIZE DYNAMIC CLIENT LIST\n");
        close(server_socket);
        return 1;
    }
    printf("dynamic memory list is online\n");

    // 8. prepare the select loop
    fd_set read_fds;
    int max_sd;

    printf("entering the main server loop... \n");

    while(1){
        // step 1 : start a fresh loop
        FD_ZERO(&read_fds);

        // step 2 : add the server socket to the list
        FD_SET(server_socket, &read_fds);
        max_sd = server_socket;

        // step 3 : loop through the connected clients and add them to the list
        for(int i = 0; i < clients.len; i++){
            struct Client *c = (struct Client *)((char*)clients.b + (i * clients.element_size));
            int current_client_socket = c->socket;

            // add the client to the set if it is valid
            if (current_client_socket > 0){
                FD_SET(current_client_socket, &read_fds);
            }

            if(current_client_socket > max_sd) {
                max_sd = current_client_socket;
            }
        }
        
        // step 4 : multiplexing
        if(select(max_sd + 1, &read_fds, NULL, NULL, NULL) < 0 ) {
            printf("ERROR : select() failed. \n");
            break;
        }

        // step 5 : did someone ask to join on the server
        if(FD_ISSET(server_socket, &read_fds)) {
            int new_socket = accept(server_socket, NULL, NULL);
            if(new_socket < 0){
                printf("WARNING : FAILED TO ACCEPT THE NEW GUY\n");
            } else {
                struct Client new_client;
                new_client.socket = new_socket;
                new_client.uuid = clients.len + 1;
                strcpy(new_client.name, "ANONYMOUS"); // DEFAULT NAME

                // put the new guy in the list
                if(vec_push(&clients, &new_client) == 0){
                    printf("new user fel bo7eera! SOCKET : %d ||| Total Online : %d\n ", new_socket, clients.len);
                } else {
                    printf("ERROR : MEMORY IS FULL, HANW23AK\n");
                    close(new_socket);
                }
            }
        }

        // step 6: loop through and see who sent a message
        for(int i = 0; i < clients.len; i++){
            struct Client *c = (struct Client *)((char *)clients.b + (i * clients.element_size));
            int current_client_socket = c->socket;

            if(FD_ISSET(current_client_socket, &read_fds)) {
                struct DataPacket incoming_packet;

                int bytes_received = recv(current_client_socket, (char*)&incoming_packet, sizeof(struct DataPacket), 0);
                
                if(bytes_received <= 0 || incoming_packet.type == PCKT_LEAVE){
                    printf("user on %d socket disconnected. \n", current_client_socket);
                    close(current_client_socket); 
                    
                    vec_swap_remove(&clients, i);
                    i--;
                } else {
                    // step 8 : broadcast message to everyone
                    for(int j = 0; j < clients.len; j++){
                        struct Client *other = (struct Client*)((char *)clients.b + (j * clients.element_size));
                        if (other->socket != current_client_socket){
                            send(other->socket, (char*)&incoming_packet, sizeof(struct DataPacket), 0);
                        } 
                    }
                }
            }
        }
    }
     
    printf("shutting the server down....\n");
    vec_free(&clients);
    close(server_socket);
    return 0;
}
