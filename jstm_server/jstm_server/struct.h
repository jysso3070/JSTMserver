#pragma once
#include <chrono>
#include <random>

enum EVENT_TYPE {
	EV_RECV, EV_SEND, EV_MOVE, EV_TEST, EV_MONSTER_THREAD_RUN, EV_GEN_1stWAVE_MONSTER, 
	EV_MONSTER_DEAD, EV_MONSTER_NEEDLE_TRAP_COLLISION, EV_MONSTER_SLOW_TRAP_COLLISION,
	EV_CHECK_WAVE_END, EV_GEN_MONSTER, EV_MONSTER_ATTACK, EV_PLAYER_DAMAGE_COOLTIME
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
	bool damageCooltime;

	mutex player_info_lock;
	mutex roomList_lock;

	DirectX::XMFLOAT3 get_pos() {
		return DirectX::XMFLOAT3(player_world_pos._41, player_world_pos._42, player_world_pos._43);
	}
};

struct GAME_ROOM {
	bool enable;
	short room_number;
	char room_state;
	short wave_count;
	short stage_number;
	short portalLife;
	int players_id[4];
	bool wave_on;
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


#define MONSTER_GEN_DISTANCE	70.f

static default_random_engine dre;
static uniform_int_distribution<> stage1_start123(0, 300);
static uniform_int_distribution<> stage1_start456(0, 300);

static uniform_int_distribution<> stage1_check1(0, 200);
static uniform_int_distribution<> stage1_check2(0, 200);
static uniform_int_distribution<> stage1_check2_y(0, 300);
static uniform_int_distribution<> stage1_check3(0, 300);