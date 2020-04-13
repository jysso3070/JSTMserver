#pragma once
#define SERVER_IP "127.0.0.1"

class network_manager
{
public:
	network_manager();
	~network_manager();

	void init_socket();
	SOCKET rq_connect_server(const char * server_ip);


public:
	WSADATA m_WSAData;
	SOCKET m_serverSocket;
	sockaddr_in m_serverAddr;
};

