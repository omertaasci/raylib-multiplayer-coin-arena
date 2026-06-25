#ifndef NETWORK_H
#define NETWORK_H

#include "../shared/protocol.h"

#ifdef _WIN32
#include <WinSock2.h>
typedef SOCKET SocketHandle;
#else
typedef int SocketHandle;
#endif

typedef struct NetworkClient
{
    SocketHandle socket; // server socket
    int connected; // 1 connected, 0 not connected
    int player_id; // player id given by server
} NetworkClient;

// starts systems like winsock
int Network_InitSystem(void);

// closes the network system
void Network_ShutdownSystem(void);

// reset the client structure to its initial state.
void NetworkClient_Init(NetworkClient *client);

// connects to server and sends HELLO/WELCOME messages
int NetworkClient_Connect(NetworkClient *client, const char *ip, int port);

// closes the connection
void NetworkClient_Close(NetworkClient *client);

#endif