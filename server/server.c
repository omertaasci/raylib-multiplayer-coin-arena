#include "server.h"
#include "../shared/protocol.h"

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
#endif

// sends all bytes untill the entire buffer is sent
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

// rreceives all bytes until the requested size is fully received
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

//send a packet and optional payload
static int SendPacket(SocketHandle socket, int type, const void *payload, int payloadSize)
{
    PacketHeader header;
    header.type = type;
    header.size = payloadSize;

    if (!SendAll(socket, &header, sizeof(header))) // send header first
    {
        return 0;
    }

    if (payloadSize > 0 && payload != NULL) // if payload existc send it too
    {
        if (!SendAll(socket, payload, payloadSize))
        {
            return 0;
        }
    }
    return 1;
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
    serverAddress.sin_addr.s_addr = INADDR_ANY; // accept connevtion from any ip
    serverAddress.sin_port = htons((unsigned short)port); // set port number

    // bind socket to ip address and port
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

    printf("[SERVER] listening on port %d\n", port);

    SocketHandle clientSocket = accept(listenSocket, NULL, NULL); // wait for client connection

    if (clientSocket == 0) // check if accept connection failed
    {
        printf("[SERVER] accept failed\n");
        return 0;
    }

    printf("[SERVER] client connected\n");

    PacketHeader header; // incoming packet header

    if (!RecvAll(clientSocket, &header, sizeof(header))) //read packet header
    {
        printf("[SERVER] failed to read header\n");
        return 0;
    }

    // verify packet type and expected size
    if (header.type != PACKET_HELLO || header.size != sizeof(HelloPacket))
    {
        printf("[SERVER] unexpected packet\n");
        return 0;
    }

    HelloPacket hello; // scturct for hello packet

    if (!RecvAll(clientSocket, &hello, sizeof(hello))) // read hello packet data
    {
        printf("[SERVER] failed to read HELLO\n");
        return 0;
    }

    printf("[SERVER] received HELLO version=%d\n", hello.version); // client version
    
    WelcomePacket welcome;
    welcome.player_id = 1; // assign player id

    //send welcome packet to client
    SendPacket(clientSocket, PACKET_WELCOME, &welcome, sizeof(welcome));

    printf("[SERVER] sent WELCOME player_id=1\n");

#ifdef _WIN32
    // close sockets on windows
    closesocket(clientSocket);
    closesocket(listenSocket);
    // shutdown winsock
    WSACleanup();
#else
    // close sockets on linux 
    close(clientSocket);
    close(listenSocket);
#endif
    return 1; // server finished successfully
}