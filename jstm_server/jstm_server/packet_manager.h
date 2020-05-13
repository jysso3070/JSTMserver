#pragma once

#include "header.h"
#include "struct.h"

class packet_manager
{
public:
	packet_manager();
	~packet_manager();

public:
	void send_packet(int client_id, SOCKET client_socket, void* buf);

	void send_id_packet(int client_id, SOCKET client_socket);
	void t_send_pos_packet(int client_id, SOCKET client_socket, short p_x, short p_y);
	void send_put_player_packet(int client_id, SOCKET client_socket, int new_player_id);
	void send_pos_packet(int client_id, SOCKET client_socket, int mover_id, DirectX::XMFLOAT4X4 player_pos, short animation_state);
	void send_remove_player_packet(int client_id, SOCKET client_socket, int leave_player_id);
	void send_room_info_pakcet(int client_id, SOCKET client_socket, GAME_ROOM game_room);
	void send_join_room_ok(int client_id, SOCKET client_socket, short room_number);
	void send_trap_info_packet(int client_id, SOCKET client_socket, DirectX::XMFLOAT4X4 trap_pos, char trap_type);


	void error_display(const char* msg, int err_no); // 에러 출력 함수

private:
	
};

