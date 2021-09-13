#ifndef CLEINT_HEADER
#define CLEINT_HEADER
#include <iostream>
#include <fstream>
#include <string>
#include <thread>

#include "socket.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable: 4996)

class Client : virtual public Socket
{
public:
    SOCKET Connection;
    Client() {}

    virtual ~Client() {}

    virtual void info() {
        std::cout << "Client instance" << std::endl;
    }

    void packetHandler() {
        Packet packettype;
        while (true) {

            try
            {
                recv(Connection, (char*)&packettype, sizeof(Packet), NULL);

                if (!ProcessPacket(0, packettype)) {
                    break;
                }
            }
            catch (const std::exception a)
            {
                log(a.what());
            }

        }
        closesocket(Connection);
    }

    std::string getFileName(std::string pathToFile) {
        std::string delim = "/";
        size_t pos = 0;
        size_t base_str_size = pathToFile.size();
        size_t delim_size = delim.size();
        std::string fileName;
        while (pos < base_str_size) {
            fileName = fileName.assign(pathToFile, pos, pathToFile.find(delim, pos) - pos);
            pos += fileName.size() + delim_size;
        }

        return fileName;
    }


    void InputHandler() {
        std::string msg;
        char* token1;
        Packet packettype = P_ChatMessage;

        std::getline(std::cin, msg);

        if (msg.size() == 0) return;

        char* pch = strtok_s((char*)msg.c_str(), " ", &token1);

        if (std::strcmp(pch, "sendfile") == 0) {
            char* pathTofile = strtok_s(token1, " ", &token1);
            std::ifstream file(pathTofile);
            std::string fileName = getFileName(pathTofile);
            std::cout << "Start copy FILE " << fileName << std::endl;

            if (!file.is_open()) {
                std::cout << "File: " << pathTofile << " Not found" << std::endl;
                return;
            }

            std::string str;
            packettype = S_Start_Transfer_File;
            SendMessageTo(Connection, packettype, fileName);
            while (!file.eof()) {
                std::getline(file, str);
                SendMessageTo(Connection, packettype, str);
            }
            SendMessageTo(Connection, S_End_Transfer_File, "");
        }
        else {
            int msg_size = msg.size();
            SendMessageTo(Connection, packettype, msg);
        }
    }

    bool ProcessPacket(int index, Packet packettype) {
    
        switch (packettype) {
        case P_ChatMessage:
        {
            int msg_size;
            recv(Connection, (char*)&msg_size, sizeof(int), NULL);
            char* msg = new char[msg_size + 1];
            msg[msg_size] = '\0';
            recv(Connection, msg, msg_size, NULL);
            std::cout << msg << std::endl;
            delete[] msg;
            break;
        }
        default:            
            break;
        }

        return true;
    }

    virtual void connection() {
        Connection = socket(AF_INET, SOCK_STREAM, NULL);
        if (connect(Connection, (SOCKADDR*)&addrInstance, sizeof(addrInstance)) != 0) {
            std::cout << "Error: failed connect to server.\n";
            return;
        }
        std::cout << "Connected!\n";

        std::thread t(([this]() {
                this->packetHandler();
            }
        ));
        t.detach();

        while (true) {
            InputHandler();
            Sleep(10);
        }
    }


};
#endif