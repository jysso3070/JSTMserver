#pragma once

#define MAX_BUFFER 1024

// user_state
#define STATE_playing_game	0
#define STATE_in_lobby		1
#define STATE_in_room		2

#define SC_SEND_ID			1
#define SC_POS				2
#define SC_SEND_ROOM_LIST	3	


#define SERVER_PORT	3500
#define CS_LEFT		1
#define CS_RIGHT	2
#define CS_UP		3
#define CS_DOWN		4
#define CS_MAKE_ROOM	5
#define CS_REQUEST_JOIN_ROOM	6

#pragma pack(push ,1)

// server to client
struct sc_packet_send_id {
	char size;
	char type;
	int id;
};

struct sc_packet_pos {
	char size;
	char type;
	int id;
	short x, y;

	DirectX::XMFLOAT4X4 world_pos;
};

struct sc_pakcet_room_list {
	char size;
	char type;
	short room_num;
	int host_id;
	int guest_id;
};


// client to server
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
	int id;
	int room_number;
};


#pragma pack (pop) 