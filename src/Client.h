//
// Created by Nickt on 18/05/2022.
//

#ifndef NETWORKMESSENGER_CLIENT_H
#define NETWORKMESSENGER_CLIENT_H

#define DEFAULT_PORT "54321"
#define DEFAULT_BUFLEN 256

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

class Client {
public:
    Client(const char *ip);
    ~Client();

    int init();
    int connectServer();
    int transmission();
    int sendData();
    int receiveData();
    int close();

private:
    struct addrinfo *result = NULL;
    struct addrinfo *ptr = NULL;
    struct addrinfo hints;

    const char *ip;
    const char *sendbuf;
    char recvbuf[DEFAULT_BUFLEN];

    int recvbuflen = DEFAULT_BUFLEN;
    int iResult;

    std::string input;

    WSADATA wsaData;
    SOCKET connectSocket = INVALID_SOCKET;
};

#endif //NETWORKMESSENGER_CLIENT_H
