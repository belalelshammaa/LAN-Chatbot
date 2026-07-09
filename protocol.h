#ifndef PROTOCOL_H
#define PROTOCOL_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
//the windows networking libararies
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
//this is what tells the compiler to link the windows socket automatically
#pragma comment(lib, "ws2_32.lib")


#define PAYLOAD_SIZE 1024
#define DEFAULT_PORT 8888
    
    
    
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