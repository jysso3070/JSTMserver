#include "stdafx.h"
#include "network_manager.h"


network_manager::network_manager()
{
}


network_manager::~network_manager()
{
}

void network_manager::init_socket()
{
	WSAStartup(MAKEWORD(2, 0), &m_WSAData);	//  네트워크 기능을 사용하기 위함, 인터넷 표준을 사용하기 위해
	m_serverSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
	memset(&m_serverAddr, 0, sizeof(SOCKADDR_IN));
	m_serverAddr.sin_family = AF_INET;
	m_serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_IP, &m_serverAddr.sin_addr);// ipv4에서 ipv6로 변환	
}

SOCKET network_manager::connect_server()
{
	connect(m_serverSocket, (struct sockaddr *)&m_serverAddr, sizeof(m_serverAddr));
	return m_serverSocket;
}
