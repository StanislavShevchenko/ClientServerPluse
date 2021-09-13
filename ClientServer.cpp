#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>

#include  "factoryClientServer.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable: 4996)

int main()
{
    SocketType type = ClientInstance;
    std::string newType = "";

    std::cout << "Please set you'r instance (server/client): ";
    std::cin >> newType;

    SOCKADDR_IN addr;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(1111);
    addr.sin_family = AF_INET;


    if (newType == "server") {
        type = ServerInstance;
    }

    Socket* socket = NewSocket::createSocket(type);

    socket->open(addr);
    socket->info();
    socket->connection();

    system("pause");
    return 0;    
}