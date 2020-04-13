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
	void Initialize();	// 서버 초기화
	void make_thread();	// 스레드 생성


	void init_DB();
	void init_socket();

	void do_accept_thread(); // 소켓 accept를 받는 스레드함수
	void do_worker_thread(); // 주 워커 스레드
	void do_eventTimer_thread();

	void add_event_to_eventTimer(EVENT &ev);

	void t_process_player_move(int id, void *buff);
	void process_player_move(int id, void *buff);
	void process_make_room(int id);

	void send_all_room_list(int id);
	void get_player_db(); // database_manager에 있는 DBlist 가져오기


	void process_packet(int id, void *buff);
	

	// 패킷 송신
	void send_id_packet(int id);
	void send_pos_packet(int id);

	//void do_recv();

private:
	packet_manager *m_packet_manager = NULL;
	database_manager *m_database_manager = NULL;
	server_manager *m_server_manager = NULL;
	
	HANDLE m_iocp_Handle; // iocp 핸들값
	int m_new_user_id;
	short m_new_room_num;

	SOCKET m_accept_socket = NULL;

	Concurrency::concurrent_unordered_map<int, PLAYER_INFO*> m_player_info; // 플레이어 정보 맵(concurrent_unordered_map)
	priority_queue <EVENT> m_eventTimer_queue; // 우선순위 타이머 큐
	mutex m_eventTimer_lock;

	list<GAME_ROOM> m_list_game_room;
	list<PLAYER_DB> m_list_player_db;
};

