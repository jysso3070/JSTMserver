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
	WSAStartup(MAKEWORD(2, 0), &m_WSAData);	//  ��Ʈ��ũ ����� ����ϱ� ����, ���ͳ� ǥ���� ����ϱ� ����
	m_serverSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
}

SOCKET network_manager::rq_connect_server(const char * server_ip)
{
	memset(&m_serverAddr, 0, sizeof(SOCKADDR_IN));
	m_serverAddr.sin_family = AF_INET;
	m_serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, server_ip, &m_serverAddr.sin_addr);// ipv4���� ipv6�� ��ȯ	
	connect(m_serverSocket, (struct sockaddr *)&m_serverAddr, sizeof(m_serverAddr));
	return m_serverSocket;
}
