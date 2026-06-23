#ifndef PROTOCOL_H
#define PROTOCOL_H

// the default port that server will listen on
#define SERVER_PORT 7777

// packet types
// client and server should agree based on these values
typedef enum PacketType
{
    PACKET_HELLO = 1, // client -> server
    PACKET_WELCOME = 2, // server -> client
    PACKET_INPUT = 3, // will be used later
    PACKET_WORLD_STATE = 4, // will be used later
    PACKET_DISCONNECT = 5 // will be used later
} PacketType; 

// all packets use this header
// type tells which packet it is
// it tells how many bytes of payload come after the size header
typedef struct PacketHeader
{
    int type;
    int size;
} PacketHeader;

// first packet that client sends
typedef struct HelloPacket
{
    int version;
} HelloPacket;

// the packet server returns to client
typedef struct WelcomePacket
{
    int player_id;
} WelcomePacket;

#endif