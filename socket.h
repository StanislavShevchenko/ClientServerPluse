#ifndef BASE_TRANSPORT_CLIENT_SERVER_HEADER
#define BASE_TRANSPORT_CLIENT_SERVER_HEADER

#include <iostream>

enum Packet {
    P_ChatMessage,
    S_Start_Transfer_File,
    S_End_Transfer_File,
};

class Socket
{
protected:
    int errorCode = -1;
    SOCKADDR_IN addrInstance;
public:
    Socket() {}
    virtual ~Socket() {}
    virtual void info() = 0;
    virtual void connection() = 0;
    virtual bool ProcessPacket(int index, Packet packettype) = 0;

    bool open(SOCKADDR_IN addr) {
        WSAData wsaData;
        WORD DLLVersion = MAKEWORD(2, 1);
        if (WSAStartup(DLLVersion, &wsaData) != 0) {
            std::cout << "Error" << std::endl;
            exit(1);
        }
        addrInstance = addr;
        return true;
    }

    void log(std::string text) {
        std::cout << "System: " << text << std::endl;
    }

    virtual void SendMessageTo(SOCKET& connection, Packet msgtype, std::string msg) {
        int msg_size = msg.size();
        send(connection, (char*)&msgtype, sizeof(Packet), NULL);
        send(connection, (char*)&msg_size, sizeof(int), NULL);
        send(connection, msg.c_str(), msg_size, NULL);
    }
};
#endif