#pragma once
#include "header.h"

class Server_manager
{
public:
	Server_manager();
	~Server_manager();

	void get_server_ipAddress();
	void get_cpu_count();
	void socket_error_display(const char * msg, int err_no);  // ���� ��� �Լ�
	void socket_error_quit(const char * msg, int err_no);
};

