#ifndef NETWORK_H
#define NETWORK_H

#include "../shared/protocol.h"

#ifdef _WIN32

typedef unsigned long long SocketHandle;

#else

typedef int SocketHandle;

#endif

typedef struct NetworkClient
{
    SocketHandle socket;
    int connected; // 1 connected 0 not connected
    int player_id;

} NetworkClient;

int NetworkClient_SendInput(NetworkClient *client, InputPacket input); // sends input packet

int NetworkClient_ReceiveWorldState(NetworkClient *client, WorldStatePacket *outState); // gets game state from server

int Network_InitSystem(void); // starts the winsock for windows

void Network_ShutdownSystem(void); // closes the socket

void NetworkClient_Init(NetworkClient *client);

int NetworkClient_Connect(NetworkClient *client, const char *ip, int port);

void NetworkClient_Close(NetworkClient *client);

#endif