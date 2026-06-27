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
    int connected;
    int player_id;

} NetworkClient;

int Network_InitSystem(void);
void Network_ShutdownSystem(void);

void NetworkClient_Init(NetworkClient *client);

int NetworkClient_Connect(NetworkClient *client, const char *ip, int port);

void NetworkClient_Close(NetworkClient *client);

#endif