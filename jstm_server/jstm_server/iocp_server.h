#pragma once
#include "header.h"
#include "packet_manager.h"
#include "database_manager.h"
#include "server_manager.h"

class packet_manager;
class database_manager;
class server_manager;

class iocp_server
{
public:
	iocp_server();
	~iocp_server();

public:
	void Initialize();	// ���� �ʱ�ȭ
	void make_thread();	// ������ ����


	void init_DB();
	void init_socket();

	void do_accept_thread(); // ���� accept�� �޴� �������Լ�
	void do_worker_thread(); // �� ��Ŀ ������
	void do_eventTimer_thread();

	void add_event_to_eventTimer(EVENT &ev);

	void t_process_player_move(int id, void *buff);
	void process_player_move(int id, void *buff);
	void process_make_room(int id);
	void process_join_room(int id, void *buff);

	void send_all_room_list(int id);
	void get_player_db(); // database_manager�� �ִ� DBlist ��������
	void process_leave_client(int leaver_id);


	void process_packet(int id, void *buff);
	

	// ��Ŷ �۽�
	void send_id_packet(int id);
	void send_pos_packet(int id);

	//void do_recv();

private:
	packet_manager *m_packet_manager = NULL;
	database_manager *m_database_manager = NULL;
	server_manager *m_server_manager = NULL;
	
	HANDLE m_iocp_Handle; // iocp �ڵ鰪
	int m_new_user_id;
	short m_new_room_num;

	SOCKET m_accept_socket = NULL;

	Concurrency::concurrent_unordered_map<int, PLAYER_INFO*> m_map_player_info; // �÷��̾� ���� ��(concurrent_unordered_map)
	priority_queue <EVENT> m_eventTimer_queue; // �켱���� Ÿ�̸� ť
	mutex m_eventTimer_lock;

	map<int, GAME_ROOM> m_map_game_room;	// room����

	list<PLAYER_DB> m_list_player_db;	// DB����
};

