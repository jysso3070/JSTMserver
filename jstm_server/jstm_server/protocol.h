#pragma once
#include <DirectXMath.h>

#define MAX_BUFFER 1024
#define SERVER_PORT	3500

// user_state
#define STATE_playing_game	0
#define STATE_in_lobby		1
#define STATE_in_room		2

#define SC_SEND_ID			1
#define SC_POS				2
#define SC_SEND_ROOM_LIST	3	
#define SC_PUT_PLAYER		4
#define SC_REMOVE_PLAYER	5
#define SC_TRAP_INFO		6



#define CS_LEFT		1
#define CS_RIGHT	2
#define CS_UP		3
#define CS_DOWN		4
#define CS_MAKE_ROOM	5
#define CS_REQUEST_JOIN_ROOM	6
#define CS_POS		7
#define CS_TEST		8
#define CS_INSTALL_TRAP	9

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
	short room_num;
	int player_1_id;
	int player_2_id;
	int player_3_id;
	int player_4_id;
};

struct sc_packet_trap_info {
	char size;
	char type;
	int id;
	char trap_type;
	DirectX::XMFLOAT4X4 trap_world_pos;
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

struct cs_packet_requset_join_room {
	char size;
	char type;
	int joiner_id;
	short room_number;
};

struct cs_packet_pos {
	char size;
	char type;
	int id;
	DirectX::XMFLOAT4X4 player_world_pos;
};

struct cs_packet_install_trap {
	char size;
	char type;
	int id;
	char trap_type;
	DirectX::XMFLOAT4X4 trap_world_pos;
};


#pragma pack (pop) 