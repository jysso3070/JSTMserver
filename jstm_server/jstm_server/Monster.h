#pragma once
#include "header.h"
#include "direct_vector.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

class Monster
{
public:
	Monster();
	~Monster();
	
	void set_id(int id);
	void set_4x4position(XMFLOAT4X4 pos);
	void set_monster_type(char monster_type);
	void set_monster_isLive(bool flag);

	int get_monster_id();
	char get_monster_type();
	XMFLOAT4X4 get_4x4position();
	XMFLOAT3 get_right() { return XMFLOAT3(m_4x4position._11, m_4x4position._12, m_4x4position._13); }
	XMFLOAT3 get_up() { return XMFLOAT3(m_4x4position._21, m_4x4position._22, m_4x4position._23); }
	XMFLOAT3 get_look() { return XMFLOAT3(m_4x4position._31, m_4x4position._32, m_4x4position._33); }
	XMFLOAT3 get_3x3position() { return XMFLOAT3(m_4x4position._41, m_4x4position._42, m_4x4position._43); }

	void move_forward(float distance);
	void set_aggro_direction(XMFLOAT3 target_postion); // 어그로 대상으로 방향벡터 설정


private:
	int m_id;
	char m_monster_type;
	bool m_isLive = false;

	XMFLOAT4X4 m_4x4position;
};

