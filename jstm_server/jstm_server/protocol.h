#pragma once
#include <DirectXMath.h>

struct GAME_ROOM {
	short room_number;
	int players_id[4];
};


#define MAX_BUFFER 1024
#define SERVER_PORT	3500
#define MONSTER_ID_START	100

// user_state
#define PLAYER_STATE_default		0
#define PLAYER_STATE_playing_game	1
#define PLAYER_STATE_in_lobby		2
#define PLAYER_STATE_in_room		3

#define SC_SEND_ID			1
#define SC_POS				2
#define SC_SEND_ROOM_LIST	3	
#define SC_PUT_PLAYER		4
#define SC_REMOVE_PLAYER	5
#define SC_TRAP_INFO		6
#define SC_JOIN_ROOM_OK		7



#define CS_LEFT		1
#define CS_RIGHT	2
#define CS_UP		3
#define CS_DOWN		4
#define CS_MAKE_ROOM	5
#define CS_REQUEST_JOIN_ROOM	6
#define CS_POS		7
#define CS_TEST		8
#define CS_INSTALL_TRAP		9
#define CS_CLIENT_STATE_CHANGE	10

#pragma pack(push ,1)

// server to client
struct sc_packet_send_id {
	char size;
	char type;
	int id;
};

struct sc_packet_put_player {
	char size;
	char type;
	int new_player_id;
};

struct sc_packet_pos {
	char size;
	char type;
	int mover_id;
	short x, y;
	short animation_state;
	DirectX::XMFLOAT4X4 world_pos;
};

struct sc_packet_remove_player {
	char size;
	char type;
	int leave_player_id;
};

struct sc_pakcet_room_info {
	char size;
	char type;
	GAME_ROOM game_room;
};

struct sc_packet_trap_info {
	char size;
	char type;
	int id;
	char trap_type;
	DirectX::XMFLOAT4X4 trap_world_pos;
};

struct sc_packet_join_room_ok {
	char size;
	char type;
	int id;
	short room_number;
};


// client to server

struct cs_packet_test {
	char size;
	char type;
	int id;
};

struct cs_packet_left {
	char size;
	char type;
};

struct cs_packet_right {
	char size;
	char type;
};

struct cs_packet_up {
	char size;
	char type;
};

struct cs_packet_down {
	char size;
	char type;
};

struct cs_packet_make_room {
	char size;
	char type;
	int id;
};

struct cs_packet_request_join_room {
	char size;
	char type;
	int joiner_id;
	short room_number;
};

struct cs_packet_pos {
	char size;
	char type;
	int id;
	short animation_state;
	DirectX::XMFLOAT4X4 player_world_pos;
};

struct cs_packet_install_trap {
	char size;
	char type;
	int id;
	char trap_type;
	DirectX::XMFLOAT4X4 trap_world_pos;
};

struct cs_packet_client_state_change {
	char size;
	char type;
	int id;
	char change_state;
};


#pragma pack (pop) 