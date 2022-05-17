#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define DEFAULT_PORT "54321"
#define DEFAULT_BUFLEN 512

int main(int argc, char *argv[]) {
    std::cout << "Client starting ... " << std::endl;

    int recvbuflen = DEFAULT_BUFLEN;

    const char* sendbuf = "test";
    char recvbuf[DEFAULT_BUFLEN];

    WSADATA wsaData;
    int iResult;

    struct addrinfo *result = NULL;
    struct addrinfo *ptr = NULL;
    struct addrinfo hints;

    SOCKET connectSocket = INVALID_SOCKET;

    // Initialise Winsock
    /**
     * WSAStartup initiates Winsock DLL
     * MAKEWORD concatenates two 1-byte words into one 16-bit word
     * MAKEWORD requests version 2.2 of Winsock
     * If iResult != 0, WSAStartup has failed
     */
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    // Fills a block of memory with zeroes
    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_UNSPEC;        // Unspecified address family can return IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;    // Application requests sock type to be a stream for TCP
    hints.ai_protocol = IPPROTO_TCP;

    // Request IP address of server from command line
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Point to result addrinfo struct
    ptr=result;

    // Setup socket to use for connection to server
    connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    // Check connectSocket is now setup correctly
    if (connectSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Connect to server
    iResult = connect( connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

    // Check if connection was successful
    if (iResult == SOCKET_ERROR) {
        closesocket(connectSocket);
        connectSocket = INVALID_SOCKET;
    }

    freeaddrinfo(result);

    // Check if connection failed
    if (connectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Send initial buffer to server
    iResult = send(connectSocket, sendbuf, (int) strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);

    // shutdown the connection for sending since no more data will be sent
    // the client can still use the ConnectSocket for receiving data
    iResult = shutdown(connectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    // Receive data until the server closes the connection
    do {
        iResult = recv(connectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
            printf("Bytes received: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed: %d\n", WSAGetLastError());
    } while (iResult > 0);

    // shutdown the send half of the connection since no more data will be sent
    iResult = shutdown(connectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    // Cleanup resources
    closesocket(connectSocket);
    WSACleanup();

    return 0;
}
