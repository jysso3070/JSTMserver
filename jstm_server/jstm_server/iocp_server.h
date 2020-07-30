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
#include "direct_vector.h"
#include "monster_path.h"

//class packet_manager;
//class Database_manager;
//class Server_manager;

class Iocp_server
{
public:
	Iocp_server();
	~Iocp_server();

public:
	void serverInitialize();	// 서버 초기화
	void make_thread();	// 스레드 생성


	void init_DB();
	void init_socket();

	// thread
	void do_accept_thread(); // 소켓 accept를 받는 스레드함수
	void do_worker_thread(); // 주 워커 스레드
	void do_eventTimer_thread();
	void do_monster_move(const short room_number);
	void do_packet_count();

	// event queue add
	void add_event_to_eventTimer(EVENT &ev);

	void t_process_player_move(const int& id, void *buff);	// 테스트용
	void process_player_move(const int& id, void *buff);	// 플레이어 움직임
	void process_make_room(const int& id);					// 플레이어 방 생성
	void process_join_room(const int& id, void *buff);		// 플레이어 방 입장
	void process_client_state_change(const int& id, void *buff);	// 플레이어 상태 변경
	void process_install_trap(const int& id, void *buff);	// 함정설치

	void process_game_start(const short& room_number, const short& stage_number);
	void process_game_end(const short& room_number, const bool& clearFlag);

	void check_wave_end(const short& room_number);
	void add_monster_dead_event(const short& room_number, const short& monster_id);

	void send_all_room_list(const int& id);		// 모든 방정보 전송
	void get_player_db(); // database_manager에 있는 DBlist 가져오기
	void process_disconnect_client(const int& leaver_id);
	void process_nameLogin(const int& id, void *buff);
	void check_monster_attack(const short& room_number, const short& monster_id);

	void process_packet(const int& id, void *buff);

	void process_gen_monster(const short& room_number, const short& stage_number); // 몬스터 리젠
	
	//
	void do_monster_thread();

private:
	// class
	packet_manager *m_Packet_manager = NULL;
	Database_manager *m_Database_manager = NULL;
	Server_manager *m_Server_manager = NULL;
	Collision *m_Collision = NULL;
	Timer *m_Timer = NULL;
	
	// iocp id
	HANDLE m_iocp_Handle; // iocp 핸들
	int m_new_user_id;
	short m_new_room_num;
	short m_new_trap_id;
	Concurrency::concurrent_unordered_map<short, short> m_map_trapIdPool;

	SOCKET m_accept_socket = NULL;

	// STL container
	Concurrency::concurrent_unordered_map<int, PLAYER_INFO*> m_map_player_info; // 플레이어 정보 맵(concurrent_unordered_map)
	priority_queue <EVENT> m_eventTimer_queue; // 우선순위 타이머 큐
	mutex m_eventTimer_lock;

	Concurrency::concurrent_unordered_map<short, GAME_ROOM*> m_map_game_room;	// room정보
	Concurrency::concurrent_unordered_map<short, Monster*> m_map_monsterPool;
	Concurrency::concurrent_unordered_map<short, Trap*> m_map_trap;

	list<PLAYER_DB> m_list_player_db;	// DB정보

	volatile bool m_monsterThread_run = true;

	short pakcetCount = 0;

};

