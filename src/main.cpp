#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <thread>
#include "Client.h"

#define DEFAULT_PORT "54321"
#define DEFAULT_BUFLEN 256

Client *client = nullptr;
std::string input;

int main(int argc, char *argv[]) {
    std::cout << "Client starting ... ";

    // Check if IP has been passed as program argument
    if (argc > 1)
    {
        client = new Client(argv[1]);
    } else
    {
        std::cout << "\nEnter IP to connect: " << std::endl;
        std::cin >> input;
        client = new Client(input);
    }


    std::cout << "Input: " << input << std::endl;

    if (client->init() != 0)
    {
        std::cout << "Winsock setup failed" << std::endl;
    }

    if (client->connectServer() != 0)
    {
        std::cout << "Failed to connect to server" << std::endl;
    } else
    {
        std::cout << "Connected!\n" << std::endl;

    }

    if (client->transmission() != 0)
    {
        std::cout << "Error occurred during transmission" << std::endl;
    }

    if (client->close() != 0)
    {
        std::cout << "Issues occurred during shutdown" << std::endl;
    }

    std::cout << "Closed!" << std::endl;
    return 0;
}
