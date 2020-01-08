#pragma once
#include "header.h"
#include "packet_manager.h"

class packet_manager;
class iocp_server
{
public:
	iocp_server();
	~iocp_server();

public:
	void Initialize();	// ���� �ʱ�ȭ
	void make_thread();	// ������ ����

	void do_accept_thread(); // ���� accept�� �޴� �������Լ�
	void do_worker_thread();
	void process_packet(int id, void *buff);

	// ��Ŷ �۽�
	void send_id_packet(int id);
	void send_pos_packet(int id);

	//void do_recv();

	void error_display(const char* msg, int err_no); // ���� ��� �Լ�
	void error_quit(const char * msg, int err_no);

private:
	HANDLE m_iocp_Handle; // iocp �ڵ鰪
	int m_new_user_id;

	packet_manager *m_packet_manager = NULL;
	Concurrency::concurrent_unordered_map<int, PLAYER_INFO*> m_player_info; // �÷��̾� ���� ��(concurrent_unordered_map)

};

