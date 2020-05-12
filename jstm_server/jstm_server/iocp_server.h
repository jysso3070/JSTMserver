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
	void Initialize();	// 서버 초기화
	void make_thread();	// 스레드 생성


	void init_DB();
	void init_socket();

	void do_accept_thread(); // 소켓 accept를 받는 스레드함수
	void do_worker_thread(); // 주 워커 스레드
	void do_eventTimer_thread();
	void doTempThread();

	void add_event_to_eventTimer(EVENT &ev);

	void t_process_player_move(int id, void *buff);
	void process_player_move(int id, void *buff);
	void process_make_room(int id);
	void process_join_room(int id, void *buff);
	void process_install_trap(int id, void *buff);

	void send_all_room_list(int id);
	void get_player_db(); // database_manager에 있는 DBlist 가져오기
	void process_disconnect_client(int leaver_id);


	void process_packet(int id, void *buff);
	

	// 패킷 송신
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
	HANDLE m_iocp_Handle; // iocp 핸들값
	int m_new_user_id;
	short m_new_room_num;

	SOCKET m_accept_socket = NULL;

	// STL container
	Concurrency::concurrent_unordered_map<int, PLAYER_INFO*> m_map_player_info; // 플레이어 정보 맵(concurrent_unordered_map)
	priority_queue <EVENT> m_eventTimer_queue; // 우선순위 타이머 큐
	mutex m_eventTimer_lock;

	map<short, GAME_ROOM> m_map_game_room;	// room정보
	map<short, vector<Trap>> m_map_trap;

	list<PLAYER_DB> m_list_player_db;	// DB정보


	short pakcetCount = 0;
};

