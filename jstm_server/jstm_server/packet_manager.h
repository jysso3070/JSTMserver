#pragma once

#include "header.h"

class packet_manager
{
public:
	packet_manager();
	~packet_manager();

public:
	void send_packet(int client_id, SOCKET client_socket, void* buf);

	void send_id_packet(int client_id, SOCKET client_socket);
	void t_send_pos_packet(int client_id, SOCKET client_socket, short p_x, short p_y);
	void send_put_player(int client_id, SOCKET client_socket, int new_player_id);
	void send_pos_packet(int client_id, SOCKET client_socket, DirectX::XMFLOAT4X4 player_pos);
	void send_room_list_pakcet(int client_id, SOCKET client_socket, short room_num, int host_id, int guest_id);


	void error_display(const char* msg, int err_no); // 에러 출력 함수

private:
	
};

