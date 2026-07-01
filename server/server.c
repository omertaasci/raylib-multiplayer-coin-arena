#include "server.h"
#include "../shared/protocol.h"
#include "world.h"
#include "client_session.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32 // if os is windows use this

#define WIN32_LEAN_AND_MEAN // reduces unused windows headers

#include <winsock2.h>
#include <ws2tcpip.h>

typedef SOCKET SocketHandle; // rename SOCKET type to SocketHandle

#else // if not windows use this
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#endif

// sends all bytes untill the entire buffer is sent
// because sometimes send() doesn't send the whole buffer
static int SendAll(SocketHandle socket, const void *buffer, int size)
{
    // converts generic pointer into byte pointer for byte by byte access
    const char *data = (const char *)buffer;
    
    int sentTotal = 0; // total sent data so far

    while (sentTotal < size) // keep sending until everything is sent
    {
        int sent = send(socket, data + sentTotal, size - sentTotal, 0); // send remaining bytes

        if (sent <= 0) // if sending failed or connection closed
        {
            return 0; // failure
        }
        sentTotal += sent; // add sent bytes to total
    }
    return 1; // success
}

// receives all bytes until the requested size is fully received
// because sometimes recv() doesn't receive all the buffer
static int RecvAll(SocketHandle socket, void *buffer, int size)
{
    char *data = (char *)buffer; // convert buffer into byte pointer
    int receivedTotal = 0; // total received so far

    while (receivedTotal < size) // keep receiving untill size is reached
    {
        // receive remaining bytes
        int received = recv(socket, data + receivedTotal, size - receivedTotal, 0);

        if (received <= 0) // if failed
        {
            return 0;
        }
        receivedTotal += received; // add to total
    }
    return 1;
}

// send a packet and optional payload
static int SendPacket(SocketHandle socket, int type, const void *payload, int payloadSize)
{
    PacketHeader header;
    header.type = type;
    header.size = payloadSize;

    if (!SendAll(socket, &header, sizeof(header))) // send header first
    {
        return 0;
    }

    if (payloadSize > 0 && payload != NULL) // if payload exists send it too
    {
        if (!SendAll(socket, payload, payloadSize))
        {
            return 0;
        }
    }
    return 1;
}

// wrapper function that makes it easier to send world state packet
static int SendWorldState(SocketHandle socket, WorldStatePacket *state)
{
    return SendPacket(socket, PACKET_WORLD_STATE, state, sizeof(WorldStatePacket));
}

static int ReceiveInputIfAvailable(SocketHandle socket, InputPacket *outInput)
{
    PacketHeader header;

    if (!RecvAll(socket, &header, sizeof(header)))
    {
        return 0;
    }


    if (header.type != PACKET_INPUT || header.size != sizeof(InputPacket))
    {
        return 0;
    }

    if (!RecvAll(socket, outInput, sizeof(InputPacket)))
    {
        return 0;
    }

    return 1;
}

// make socket non-blocking so recv() does not freeze server
static void SetSocketNonBlocking(SocketHandle socket) 
{
#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(socket, FIONBIO, &mode);
#else
    int flags = fcntl(socket, F_GETFL, 0);
    fcntl(socket, F_SETFL, flags | O_NONBLOCK);
#endif
}

int Server_Run(int port) // main server function
{
#ifdef _WIN32
    WSADATA wsaData; // winsock info

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) // start winsock system
    {
        printf("[SERVER] WSAStartup failed\n");
        return 0;
    }
#endif
    SocketHandle listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // create tcp socket

    if (listenSocket == 0) // check if socket creation failed
    {
        printf("[SERVER] socket create failed\n");
        return 0;
    }

    struct sockaddr_in serverAddress; // structure that stores server address information
    memset(&serverAddress, 0, sizeof(serverAddress)); // clear the structure memory

    serverAddress.sin_family = AF_INET; // use ipv4
    serverAddress.sin_addr.s_addr = INADDR_ANY; // accept connection from any ip
    serverAddress.sin_port = htons((unsigned short)port); // set port number

    // bind socket to ip address and port
    // e.g this socket should use that port 
    if (bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        printf("[SERVER] bind failed\n");
        return 0;
    }
    
    if (listen(listenSocket, 8) < 0) // put socket into listening mode
    {
        printf("[SERVER] listen failed\n");
        return 0;
    }

    ServerWorld world;
    ClientSession sessions[MAX_PLAYERS]; // client list

    World_Init(&world); // starts the game world

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        ClientSession_Init(&sessions[i]);
    }

    printf("[SERVER] listening on port %d\n", port);
    while (1)
    {
    SocketHandle newClient = accept(listenSocket, NULL, NULL); // accepts new client connection

    #ifdef _WIN32
    if (newClient != INVALID_SOCKET)
    #else
    if (newClient >= 0)
    #endif
    {
        SetSocketNonBlocking(newClient); // because recv can freeze the server

        int playerId = World_AddPlayer(&world); // adds the new player

        if (playerId != -1)
        {
            for (int i = 0; i < MAX_PLAYERS; i++)
            {
                if (!sessions[i].active) // looks for an empty slot
                {
                    sessions[i].socket = newClient;
                    sessions[i].active = 1;
                    sessions[i].playerId = playerId;

                    WelcomePacket welcome; // packet that contains assigned player id
                    welcome.player_id = playerId;

                    SendPacket(
                        newClient,
                        PACKET_WELCOME,
                        &welcome,
                        sizeof(welcome)
                    );

                    printf("[SERVER] client joined player_id=%d\n", playerId);

                    break;
                }
            }
        }
    }

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (!sessions[i].active)
        {
            continue;
        }

        InputPacket input;

        // if client sent input update player input in world
        if (ReceiveInputIfAvailable(sessions[i].socket, &input))
        {
            World_SetInput(
                &world,
                sessions[i].playerId,
                input
            );
        }
    }

    World_Update(&world, 1.0f / 30.0f); // server tick 30 fps
    // which means server processes and updates the world 30 times per second

    WorldStatePacket state;

    World_BuildState(&world, &state);

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (sessions[i].active)
        {
            // sends the world state to all clients
            SendWorldState(
                sessions[i].socket,
                &state
            );
        }
    }

    #ifdef _WIN32
    Sleep(33); // so cpu wouldn't be %100
    #else
    usleep(33000); // so cpu wouldn't be %100
    #endif
    }


}