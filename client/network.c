#include "network.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

static int SendAll(SocketHandle socket, const void *buffer, int size)
{
    const char *data = (const char *)buffer;
    int sentTotal = 0;

    while (sentTotal < size)
    {
        int sent = send(socket, data + sentTotal, size - sentTotal, 0);

        if (sent <= 0)
        {
            return 0;
        }

        sentTotal += sent;
    }
    return 1;
}

static int SendPacket(SocketHandle socket, int type, const void *payload, int payloadSize)
{
    PacketHeader header;
    header.type = type;
    header.size = payloadSize;

    if (payload > 0 && payload != NULL)
    {
        if (!SendAll(socket, payload, payloadSize))
        {
            return 0;
        }
        
    }
    return 1;
}

int Network_InitSystem(void)
{
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0)
    {
        printf("[CLIENT] WSAStartup failed \n");
        return 0;
    }
    #endif
    return 1;
}

void Network_ShutdownSystem(void)
{
#ifdef _WIN32
    WSACleanup();
#endif
}

void NetworkClient_Init(NetworkClient *client   )
{
    client->socket = 0;
    client->connected = 0;
    client->player_id = -1;
}

int NetworkClient_Connect(NetworkClient *client, const char *ip, int port)
{
    client->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (client->socket == 0)
    {
        printf("[CLIENT] socket create failed\n");
        return 0;
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons((unsigned short)port);

    if (inet_pton(AF_INET, ip, &serverAddress.sin_addr) <= 0)
    {
        printf("[CLIENT] invalid server ip\n");
        NetworkClient_Close(client);
        return 0;
    }

    if (connect(client->socket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
    {
        printf("[CLIENT] connect failed, offline mode\n");
        NetworkClient_Close(client);
        return 0;
    }

    HelloPacket hello;
    hello.version = 1;

    if (!SendPacket(client->socket, PACKET_HELLO, &hello, sizeof(hello)))
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

    client->connected = 1;
    client->player_id = welcome.player_id;
    
    printf("[CLIENT] connected, player_id=%d\n", client->player_id);

    return 1;
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

    client->socket = 0;
    client->connected = 0;
    client->player_id = -1;
}