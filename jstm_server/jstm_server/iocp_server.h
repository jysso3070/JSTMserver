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
	void Initialize();	// 서버 초기화
	void make_thread();	// 스레드 생성

	void do_accept_thread(); // 소켓 accept를 받는 스레드함수
	void do_worker_thread();
	void process_packet(int id, void *buff);

	// 패킷 송신
	void send_id_packet(int id);
	void send_pos_packet(int id);

	//void do_recv();

	void error_display(const char* msg, int err_no); // 에러 출력 함수
	void error_quit(const char * msg, int err_no);

private:
	HANDLE m_iocp_Handle; // iocp 핸들값
	int m_new_user_id;

	packet_manager *m_packet_manager = NULL;
	Concurrency::concurrent_unordered_map<int, PLAYER_INFO*> m_player_info; // 플레이어 정보 맵(concurrent_unordered_map)

};

