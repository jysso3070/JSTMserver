#pragma once

#include "header.h"

class packet_manager
{
public:
	packet_manager();
	~packet_manager();

public:
	void send_packet(int client_id, SOCKET client_socket, void* buf);

	void error_display(const char* msg, int err_no); // 에러 출력 함수
};

