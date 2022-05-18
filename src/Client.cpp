//
// Created by Nickt on 18/05/2022.
//

#include "Client.h"
#include <stdio.h>
#include <iostream>
#include <thread>

Client::Client(const char *ip)
{
    this->ip = ip;
}

int Client::init()
{
    // Initialise Winsock
    /**
     * WSAStartup initiates Winsock DLL
     * MAKEWORD concatenates two 1-byte words into one 16-bit word
     * MAKEWORD requests version 2.2 of Winsock
     * If iResult != 0, WSAStartup has failed
     */
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("\nWSAStartup failed: %d\n", iResult);
        return 1;
    }

    // Fills a block of memory with zeroes
    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_UNSPEC;        // Unspecified address family can return IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;    // Application requests sock type to be a stream for TCP
    hints.ai_protocol = IPPROTO_TCP;

    // Request IP address of server from command line
    iResult = getaddrinfo(ip, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("\ngetaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }
}

int Client::connectServer() {
    // Point to result addrinfo struct
    ptr = result;

    // Setup socket to use for connection to server
    connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    // Check connectSocket is now setup correctly
    if (connectSocket == INVALID_SOCKET) {
        printf("\nError at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Connect to server
    iResult = connect(connectSocket, ptr->ai_addr, (int) ptr->ai_addrlen);

    // Check if connection was successful
    if (iResult == SOCKET_ERROR) {
        closesocket(connectSocket);
        connectSocket = INVALID_SOCKET;
    }

    freeaddrinfo(result);

    // Check if connection failed
    if (connectSocket == INVALID_SOCKET) {
        printf("\nUnable to connect to server!\n");
        WSACleanup();
        return 1;
    }
    return 0;
}

int Client::transmission()
{
    std::thread send_thread([this] { sendData(); });
    std::thread receive_thread([this] { receiveData(); });

    send_thread.join();
    receive_thread.join();
    return 0;
}

int Client::sendData() {
    // Read input from console and send to server
    while(std::getline(std::cin, input)) {

        // Exit loop if user enters QUIT
        if (input == "QUIT") {
            exit(1);
        }

        // Truncate message length to DEFAULT_BUFLEN
        if (input.length() > DEFAULT_BUFLEN) {
            input = input.substr(0, DEFAULT_BUFLEN);
        }
        sendbuf = input.data();

        // Send input to server
        iResult = send(connectSocket, sendbuf, (int) strlen(sendbuf), 0);
        if (iResult == SOCKET_ERROR) {
            printf("send failed: %d\n", WSAGetLastError());
            closesocket(connectSocket);
            WSACleanup();
            return 1;
        }
        printf("Bytes Sent: %ld, ", iResult);
    }
}

int Client::receiveData() {
    do {
        // Receive data until the server closes the connection
        iResult = recv(connectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);

            std::cout << "Echoed: ";
            // Print out recvbuf to console
            for (int i = 0; i < iResult; i++) {
                std::cout << recvbuf[i];
            }
            std::cout << "\n";
        } else if (iResult == 0) {
            printf("Connection closed\n");
        } else {
            printf("recv failed: %d\n", WSAGetLastError());
            exit(1);
        }
    } while(iResult > 0);
}

int Client::close()
{
    // Shutdown the connection for sending and receiving
    iResult = shutdown(connectSocket, SD_BOTH);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    // Cleanup resources
    closesocket(connectSocket);
    WSACleanup();
}