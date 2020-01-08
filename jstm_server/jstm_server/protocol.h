#pragma once

#define MAX_BUFFER 1024

#define SC_SEND_ID	1
#define SC_POS		2


#define SERVER_PORT	3500
#define CS_LEFT		1
#define CS_RIGHT	2
#define CS_UP		3
#define CS_DOWN		4

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


#pragma pack (pop) 