#include "Monster.h"



Monster::Monster()
{
}


Monster::~Monster()
{
}

void Monster::set_id(int id)
{
	m_id = id;
}

void Monster::set_4x4position(DirectX::XMFLOAT4X4 pos)
{
	m_4x4position = pos;
}

void Monster::set_monster_type(char monster_type)
{
	m_monster_type = monster_type;
}

void Monster::set_monster_isLive(bool flag)
{
	m_isLive = flag;
}

int Monster::get_monster_id()
{
	return m_id;
}

char Monster::get_monster_type()
{
	return m_monster_type;
}

DirectX::XMFLOAT4X4 Monster::get_4x4position()
{
	return m_4x4position;
}


void Monster::move_forward(float distance)
{
	m_4x4position._41 += distance;
}

void Monster::set_aggro_direction(DirectX::XMFLOAT3 target_postion)
{
	target_postion.y = -50.f;

	/*XMFLOAT3 look = Vector3::Subtract(get_3x3position(), target_postion);
	look = Vector3::Normalize(look);
	m_4x4position._31 = -look.x;
	m_4x4position._32 = -look.y;
	m_4x4position._33 = -look.z;*/
}
