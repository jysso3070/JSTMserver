#pragma once

enum EVENT_TYPE {
	EV_RECV, EV_SEND, EV_MOVE, EV_PLAYER_MOVE_NOTIFY, EV_MOVE_TARGET,
	EV_TEST, EV_MONSTER_THREAD_RUN, EV_GEN_1stWAVE_MONSTER, EV_MONSTER_DEAD, EV_MONSTER_TRAP_COLLISION, 
	EV_CHECK_WAVE_END, EV_GEN_MONSTER
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
	int DB_key;
	bool is_connect;
	char player_state;
	short room_number;
	short x, y;
	short hp;
	short gold;
	short animation_state;
	DirectX::XMFLOAT4X4 player_world_pos;

	mutex player_info_lock;
	mutex roomList_lock;

	DirectX::XMFLOAT3 get_pos() {
		return DirectX::XMFLOAT3(player_world_pos._41, player_world_pos._42, player_world_pos._43);
	}
};

struct GAME_ROOM {
	short room_number;
	char room_state;
	short wave_count;
	short stage_number;
	short portalLife;
	int players_id[4];
	mutex gameRoom_lock;
};

struct PLAYER_DB {
	int DB_key_id;
	char name[11];
	short level;
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