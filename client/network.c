#include "network.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32 // if compiling on windows

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <winsock2.h>
#include <ws2tcpip.h>

#else // linux
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

// sends all bytes until the full buffer is sent 
static int SendAll(SocketHandle socket, const void *buffer, int size)
{
    // convert generic pointer into byte pointer
    const char *data = (const char *)buffer;

    int sentTotal = 0 ;// total sent bytes

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

static int RecvAll(SocketHandle socket, void *buffer, int size)
{
    char *data = (char *)buffer;
    int receivedTotal = 0;
    while (receivedTotal < size)
    {
        int received = recv(socket, data + receivedTotal, size - receivedTotal, 0);
        
        if (received <= 0)
        {
            return 0;
        }
    receivedTotal += received;
    }
    return 1;
}

// sends packet header + optional payload
static int SendPacket(SocketHandle socket, int type, const void *payload, int payloadSize)
{
    PacketHeader header;
    header.type = type;
    header.size = payloadSize;

    if (!SendAll(socket, &header, sizeof(header)))
    {
        return 0;
    }

    if (payloadSize > 0 && payload != NULL) // if payload exists
    {
        if (!SendAll(socket, payload, payloadSize)) // send payload data
        {
            return 0; // failed
        }
        
    }
    return 1; // success
}

int Network_InitSystem(void) // initializes networking system
{
#ifdef _WIN32
    WSADATA wsaData; // holds winsock info
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); // start winsock 2.2

    if (result != 0) // check if startup failed
    {
        printf("[CLIENT] WSAStartup failed \n");
        return 0;
    }
#endif
    return 1; // success
}

void Network_ShutdownSystem(void)
{
#ifdef _WIN32
    WSACleanup();
#endif
}

void NetworkClient_Init(NetworkClient *client) // initialize client structure
{
    client->socket = 0; // no socket yet
    client->connected = 0; // not connected
    client->player_id = -1; // onvalid player id
}

int NetworkClient_Connect(NetworkClient *client, const char *ip, int port) // connect client to server
{
    client->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // create tcp socket

    if (client->socket == 0) // check if socket creation failed
    {
        printf("[CLIENT] socket create failed\n");
        return 0;
    }

    struct sockaddr_in serverAddress; // server adr struct
    memset(&serverAddress, 0, sizeof(serverAddress)); // clear struct memory

    serverAddress.sin_family = AF_INET; // use ipv4
    serverAddress.sin_port = htons((unsigned short)port); // server port

    if (inet_pton(AF_INET, ip, &serverAddress.sin_addr) <= 0) // convert ip str to binary addr
    {
        printf("[CLIENT] invalid server ip\n");
        NetworkClient_Close(client); // close socket on failure
        return 0;
    }

    // connect to server
    if (connect(client->socket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
    {
        printf("[CLIENT] connect failed, offline mode\n");

        NetworkClient_Close(client); // cleanup on failure

        return 0;
    }

    HelloPacket hello;
    hello.version = 1;

    if (!SendPacket(client->socket, PACKET_HELLO, &hello, sizeof(hello))) // send hello packet to server
    {
        printf("[CLIENT] failed to send HELLLO\n");
        NetworkClient_Close(client);
        return 0;
    }
    
    PacketHeader header;

    if (!RecvAll(client->socket, &header, sizeof(header)))
    {
        printf("[CLIENT] failed to receive header\n");
        NetworkClient_Close(client);
        return 0;
    }

    // verify expected packet type and size
    if (header.type != PACKET_WELCOME || header.size != sizeof(WelcomePacket))
    {
        printf("[CLIENT] unexpected packet\n");
        NetworkClient_Close(client);
        return 0;
    }
    
    WelcomePacket welcome;
    
    if (!RecvAll(client->socket, &welcome, sizeof(welcome)))
    {
        printf("[CLIENT] failed to receive WELCOME\n");
        NetworkClient_Close(client);
        return 0;
    }

    client->connected = 1; // mark client as connected
    client->player_id = welcome.player_id; // save assigned player id from server
    
    printf("[CLIENT] connected, player_id=%d\n", client->player_id);

    return 1; // success
}

void NetworkClient_Close(NetworkClient *client)
{
    if (client->socket != 0)
    {
#ifdef _WIN32
        closesocket(client->socket);
#else
        close(client->socket);
#endif
    }

    // reset client state
    client->socket = 0;
    client->connected = 0;
    client->player_id = -1;
}