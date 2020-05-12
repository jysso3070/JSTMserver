#pragma once
#include "header.h"
#include "packet_manager.h"
#include "database_manager.h"
#include "server_manager.h"
#include "Trap.h"
#include "Monster.h"
#include "Collision.h"
#include "timer.h"
#include "struct.h"

//class packet_manager;
//class Database_manager;
//class Server_manager;

class Iocp_server
{
public:
	Iocp_server();
	~Iocp_server();

public:
	void Initialize();	// ���� �ʱ�ȭ
	void make_thread();	// ������ ����


	void init_DB();
	void init_socket();

	void do_accept_thread(); // ���� accept�� �޴� �������Լ�
	void do_worker_thread(); // �� ��Ŀ ������
	void do_eventTimer_thread();
	void doTempThread();

	void add_event_to_eventTimer(EVENT &ev);

	void t_process_player_move(int id, void *buff);
	void process_player_move(int id, void *buff);
	void process_make_room(int id);
	void process_join_room(int id, void *buff);
	void process_install_trap(int id, void *buff);

	void send_all_room_list(int id);
	void get_player_db(); // database_manager�� �ִ� DBlist ��������
	void process_disconnect_client(int leaver_id);


	void process_packet(int id, void *buff);
	

	// ��Ŷ �۽�
	void send_id_packet(int id);
	void send_pos_packet(int id);

	//void do_recv();

private:
	// class
	packet_manager *m_Packet_manager = NULL;
	Database_manager *m_Database_manager = NULL;
	Server_manager *m_Server_manager = NULL;
	Collision *m_Collision = NULL;
	Timer *m_Timer = NULL;
	
	// iocp id
	HANDLE m_iocp_Handle; // iocp �ڵ鰪
	int m_new_user_id;
	short m_new_room_num;

	SOCKET m_accept_socket = NULL;

	// STL container
	Concurrency::concurrent_unordered_map<int, PLAYER_INFO*> m_map_player_info; // �÷��̾� ���� ��(concurrent_unordered_map)
	priority_queue <EVENT> m_eventTimer_queue; // �켱���� Ÿ�̸� ť
	mutex m_eventTimer_lock;

	map<short, GAME_ROOM> m_map_game_room;	// room����
	map<short, vector<Trap>> m_map_trap;

	list<PLAYER_DB> m_list_player_db;	// DB����


	short pakcetCount = 0;
};

