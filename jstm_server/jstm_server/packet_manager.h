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
	void send_put_player_packet(int client_id, SOCKET client_socket, int new_player_id, DirectX::XMFLOAT4X4 player_pos, short animation_state);
	void send_pos_packet(int client_id, SOCKET client_socket, int mover_id, DirectX::XMFLOAT4X4 player_pos, short animation_state);
	void send_remove_player_packet(int client_id, SOCKET client_socket, int leave_player_id);
	void send_room_info_pakcet(int client_id, SOCKET client_socket, GAME_ROOM* game_room);
	void send_make_room_ok(int client_id, SOCKET client_socket, short room_number);
	void send_join_room_ok(int client_id, SOCKET client_socket, short room_number, GAME_ROOM* game_room);
	void send_trap_info_packet(int client_id, SOCKET client_socket, short trap_id, DirectX::XMFLOAT4X4 trap_pos, char trap_type);
	void send_monster_pos(int client_id, SOCKET client_socket, MONSTER mon_arr[]);
	void send_game_end(int client_id, SOCKET client_socket, bool clearFlag);
	void send_stat_change(int client_id, SOCKET client_socket, short hp, short gold);
	void send_game_info_update(int client_id, SOCKET client_socket, short wave, short portalLife);
	void send_game_start(int client_id, SOCKET client_socket, short stage_number, short wave, short portalLife);
	void send_nameLogin_result(int client_id, SOCKET client_socket, char result);
	void send_leaveRoom_ok(int client_id, SOCKET client_socket);
	void send_wave_end(int client_id, SOCKET client_socket);


	void error_display(const char* msg, int err_no); // 에러 출력 함수

private:
	
};

