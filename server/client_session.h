#ifndef CLIENT_SESSION_H
#define CLIENT_SESSION_H

#ifdef _WIN32
#include <winsock2.h>
typedef SOCKET SocketHandle;

#else
typedef int SocketHandle;
#endif

typedef struct ClientSession
{
    SocketHandle socket;
    int active;
    int playerId;
} ClientSession;

void ClientSession_Init(ClientSession *session);

#endif