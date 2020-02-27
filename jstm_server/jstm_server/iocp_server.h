#pragma once
#include "header.h"
#include "packet_manager.h"
#include "database_manager.h"

class packet_manager;
class database_manager;

class iocp_server
{
public:
	iocp_server();
	~iocp_server();

public:
	void Initialize();	// ���� �ʱ�ȭ
	void get_server_IPaddress();
	void get_this_cpu_count();
	void make_thread();	// ������ ����


	void init_DB();

	void do_accept_thread(); // ���� accept�� �޴� �������Լ�
	void do_worker_thread(); // �� ��Ŀ ������
	void do_timer_thread();

	void add_timer(EVENT &ev);

	void process_player_move(int id, void *buff);
	void process_make_room(int id);

	void send_all_room_list(int id);


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
	short m_new_room_num;

	packet_manager *m_packet_manager = NULL;
	database_manager *m_database_manager = NULL;

	Concurrency::concurrent_unordered_map<int, PLAYER_INFO*> m_player_info; // �÷��̾� ���� ��(concurrent_unordered_map)
	priority_queue <EVENT> m_timer_queue; // �켱���� Ÿ�̸� ť
	mutex m_timer_lock;

	list<GAME_ROOM> m_list_game_room;
};

