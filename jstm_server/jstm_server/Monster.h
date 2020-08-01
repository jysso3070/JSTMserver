#pragma once
#include "header.h"
#include "direct_vector.h"
#include "monster_path.h"
#define PATH_CHECKPOINT_RANGE	50.f

using namespace DirectX;
using namespace DirectX::PackedVector;

class Monster
{
public:
	Monster();
	~Monster();
	void init();
	
	void set_id(short id);
	void set_4x4position(const XMFLOAT4X4& pos);
	void set_look(const XMFLOAT3& look);
	void set_monster_type(const char& monster_type);
	void set_isLive(const bool& flag);
	void set_pathLine(const short& line);
	void set_checkPoint(const short& checkNum);
	void set_stage_number(const short& stageNum);
	void set_position(const XMFLOAT3& position);
	void set_target_id(const int& target_id);
	void set_animation_state(const short& ani_state);
	void set_trap_cooltime(const bool& flag);
	void set_HP(const short& HP);
	void set_arrivePortal(const bool& flag);
	void set_attackCooltime(const bool& flag);

	short get_monster_id();
	char get_monster_type();
	bool get_isLive();
	XMFLOAT4X4 get_4x4position();
	XMFLOAT3 get_right() { return XMFLOAT3(m_4x4position._11, m_4x4position._12, m_4x4position._13); }
	XMFLOAT3 get_up() { return XMFLOAT3(m_4x4position._21, m_4x4position._22, m_4x4position._23); }
	XMFLOAT3 get_look() { return XMFLOAT3(m_4x4position._31, m_4x4position._32, m_4x4position._33); }
	XMFLOAT3 get_position() { return XMFLOAT3(m_4x4position._41, m_4x4position._42, m_4x4position._43); }
	int get_target_id() { return m_target_id; }
	short get_animation_state() { return m_animation_state; }
	short get_pathLine() { return m_pathLine; }
	short get_stageNum() { return m_stage_number; }
	short get_checkPoint() { return m_path_checkPoint; }
	bool get_isTrapCooltime() { return m_trap_cooltime; }
	short get_HP() { return m_hp; }
	bool get_arrivePortal() { return m_arrive_portal; }
	bool get_attackCooltime() { return m_attack_coolTime; }

	void move_forward(const float& distance);
	void set_aggro_direction( XMFLOAT3 target_postion);// 어그로 대상으로 look 방향벡터 설정

	void process_move_path();


private:
	short m_id;
	char m_monster_type;
	bool m_isLive = false;
	short m_animation_state = 1;
	short m_next_animation_state = 1;
	short m_pathLine;
	short m_stage_number;
	int m_target_id = -1;
	short m_path_checkPoint = 0;
	bool m_trap_cooltime = false;
	short m_hp;
	bool m_arrive_portal = false;
	bool m_attack_coolTime = false;

	XMFLOAT4X4 m_4x4position;
};

