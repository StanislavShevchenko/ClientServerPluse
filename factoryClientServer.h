#ifndef FACTORY_CLIENT_SERVER_HEADER
#define FACTORY_CLIENT_SERVER_HEADER

#include <iostream>
#include "socket.h"

#include "client.h"
#include "server.h"


enum SocketType { ClientInstance, ServerInstance };

class NewSocket
{
public:
    virtual void info() = 0;
    virtual ~NewSocket() {}
    static Socket* createSocket(SocketType type);
};

Socket* NewSocket::createSocket(SocketType type)
{
    Socket* p = nullptr;

    switch (type)
    {
        case ClientInstance:
            p = new Client();
            break;
        case ServerInstance:
            p = new Server();
            break;
        default:
            p = new Client();
            break;
    }
    return p;
};
#endif