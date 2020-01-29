#pragma once
#define SERVER_IP "127.0.0.1"

class network_manager
{
public:
	network_manager();
	~network_manager();

	void init_socket();
	SOCKET connect_server();
public:
	WSADATA m_WSAData;
	SOCKET m_serverSocket;
	sockaddr_in m_serverAddr;
};

