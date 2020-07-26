#pragma once
#include <DirectXMath.h>


struct MONSTER {
	short id;
	bool isLive;
	char type;
	short animation_state;
	short state;
	short hp;
	DirectX::XMFLOAT4X4 world_pos;
};


#define MAX_BUFFER			16384
#define SERVER_PORT			3500
#define MONSTER_ID_START	100
#define MAX_MONSTER			100
#define MAX_TRAP			50
#define TRAP_COLLISION_RANGE	50.f

// user_state
#define PLAYER_STATE_default		0
#define PLAYER_STATE_playing_game	1
#define PLAYER_STATE_in_lobby		2
#define PLAYER_STATE_in_room		3

// room state
#define R_STATE_in_room				0
#define R_STATE_wait_first_wave		1
#define R_STATE_wave_start			2
#define R_STATE_wave_end			3

// monster_type
#define TYPE_ORC		1
#define TYPE_SHAMAN		2
#define TYPE_STRONGORC	3
#define TYPE_RIDER		4
// monster_health
#define ORC_HP			200
#define SHAMAN_HP		150
#define STRONGORC_HP	500
#define RIDER_HP		10000



#define SC_SEND_ID			1
#define SC_POS				2
#define SC_SEND_ROOM_LIST	3	
#define SC_PUT_PLAYER		4
#define SC_REMOVE_PLAYER	5
#define SC_TRAP_INFO		6
#define SC_JOIN_ROOM_OK		7
#define SC_MAKE_ROOM_OK		8
#define SC_MONSTER_POS		9
#define SC_GAME_END			10


#define CS_LEFT					1
#define CS_RIGHT				2
#define CS_UP					3
#define CS_DOWN					4
#define CS_MAKE_ROOM			5
#define CS_REQUEST_JOIN_ROOM	6
#define CS_POS					7
#define CS_TEST					8
#define CS_INSTALL_TRAP			9
#define CS_CLIENT_STATE_CHANGE	10
#define CS_GAME_START			11
#define CS_REQUEST_NAMELOGIN	12
#define CS_SHOOT				13

#pragma pack(push ,1)

// server to client
struct sc_packet_send_id {
	unsigned short size;
	char type;
	int id;
};

struct sc_packet_put_player {
	unsigned short size;
	char type;
	int new_player_id;
};

struct sc_packet_pos {
	unsigned short size;
	char type;
	int mover_id;
	short x, y;
	short animation_state;
	DirectX::XMFLOAT4X4 world_pos;
};

struct sc_packet_remove_player {
	unsigned short size;
	char type;
	int leave_player_id;
};

struct sc_packet_room_info {
	unsigned short size;
	char type;
	short room_number;
	char room_state;
	short stage_number;
	int players_id[4];
};

struct sc_packet_trap_info {
	unsigned short size;
	char type;
	int id;
	char trap_type;
	DirectX::XMFLOAT4X4 trap_world_pos;
};

struct sc_packet_join_room_ok {
	unsigned short size;
	char type;
	int id;
	short room_number;
};

struct sc_packet_make_room_ok {
	unsigned short size;
	char type;
	int id;
	short room_number;
};

struct sc_packet_monster_pos {
	unsigned short size;
	char type;
	MONSTER monsterArr[100];
};

struct sc_packet_game_end {
	unsigned short size;
	char type;
	bool clear;
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

struct cs_packet_game_start {
	char size;
	char type;
	int id;
	short stage_number;
};

struct cs_packet_namelogin {
	char size;
	char type;
	int id;
	char name[11];
};

struct cs_packet_shoot {
	char size;
	char type;
	int id;
	//
};


#pragma pack (pop) 