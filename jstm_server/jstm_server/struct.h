#pragma once

enum EVENT_TYPE {
	EV_RECV, EV_SEND, EV_MOVE, EV_PLAYER_MOVE_NOTIFY, EV_MOVE_TARGET,
	EV_ATTACK, EV_HEAL, EV_PLAYER_RESPAWN, EV_NPC_RESPAWN, EV_MOVE_COOLTIME,
	EV_TEST
};

struct OVER_EX {		
	WSAOVERLAPPED over;
	WSABUF wsabuf[1];
	char net_buf[MAX_BUFFER];
	EVENT_TYPE event_type;
};

struct PLAYER_INFO {
	OVER_EX recv_over;
	SOCKET socket;
	int id;
	bool is_connect;

	short x, y;

	mutex player_info_lock;
};

struct PLAYER_DB {
	short DB_key_id;
	char name[11];
	short level;
};

struct GAME_ROOM {
	int room_number;
	int host_id;
	int guest_id;
};

struct EVENT
{
	int obj_id;
	chrono::high_resolution_clock::time_point wakeup_time;
	int event_type;
	int target_obj;
	constexpr bool operator < (const EVENT& left) const {
		return wakeup_time > left.wakeup_time;
	}
};