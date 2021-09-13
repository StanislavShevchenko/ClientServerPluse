#ifndef SERVER_HEADER
#define SERVER_HEADER
#include <iostream>
#include "socket.h"
#include <fstream>
#include <thread>


class Server : virtual public Socket
{
public:
	int countConnection = 10;
	int countTransferPackag = 0;
	std::ofstream file;
	std::string basePath = "c:/temp/";

	SOCKET* Connections = new SOCKET[countConnection];

	Server() {}
	virtual ~Server() {}
    virtual void info() {
		log("Server instance");
    }  
    
	void packetHandler(int index) {
		Packet packettype;
		try
		{
			while (true) {
				try
				{
					recv(Connections[index], (char*)&packettype, sizeof(Packet), NULL);

					if (!ProcessPacket(index, packettype)) {
						break;
					}
				}
				catch (const std::exception a)
				{
					log(a.what());
				}
				
			}
		}
		catch (const std::exception e)
		{
			log(e.what());
			closesocket(Connections[index]);
		}
	}

	bool ProcessPacket(int index, Packet packettype) {
		int msg_size;

		switch (packettype) {
		case P_ChatMessage:
		{

			recv(Connections[index], (char*)&msg_size, sizeof(int), NULL);
			if (msg_size > 0) {
				char* msg = new char[msg_size + 1];
				msg[msg_size] = '\0';
				recv(Connections[index], msg, msg_size, NULL);
				std::string strM = msg;
				for (int i = 0; i < countConnection; i++) {
					if (i == index) {
						continue;
					}
					SendMessageTo(Connections[i], packettype, msg);
				}
				delete[] msg;
			}

			break;
		}
		case S_Start_Transfer_File:
		{
			std::cout << "------- load file ----------" << std::endl;
			recv(Connections[index], (char*)&msg_size, sizeof(int), NULL);
			if (msg_size > 0) {

				char* msg = new char[msg_size + 1];
				msg[msg_size] = '\0';
				recv(Connections[index], msg, msg_size, NULL);
				std::string strMsg = msg;

				if (countTransferPackag == 0) {
					std::string pathTofile = basePath + strMsg;
					log("Create new file by name: " + pathTofile);
					file.open(pathTofile, std::ios_base::app);
					if (!file.is_open()) {
						log("File not open" + pathTofile);
					}
				}
				else {
					if (file.is_open()) {
						file << strMsg;
					}
				}
				delete[] msg;
				countTransferPackag++;
			}

			break;
		}
		case S_End_Transfer_File:
		{
			countTransferPackag = 0;
			log("Close transfer");
			if (file.is_open()) {
				file.close();
			}
			break;
		}
		default:			
			break;
		}

		return true;
	}

    virtual void connection() {
		log("Server run!");
		SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
		int sizeofaddr = sizeof(addrInstance);
		bind(sListen, (SOCKADDR*)&addrInstance, sizeof(addrInstance));
		listen(sListen, SOMAXCONN);
		Packet msgtype = P_ChatMessage;
		SOCKET newConnection;
		for (int i = 0; i < countConnection; i++) {
			newConnection = accept(sListen, (SOCKADDR*)&addrInstance, &sizeofaddr);

			if (newConnection == 0) {
				std::cout << "Error #2\n";
			}
			else {
				log("Client Connected!");
				std::string msg = "You connected!";
				SendMessageTo(newConnection, msgtype, msg);

				Connections[i] = newConnection;

				std::thread t([this, i]() {
						this->packetHandler(i);
					}
				);
				t.detach();
			}
		}
    }


};
#endif