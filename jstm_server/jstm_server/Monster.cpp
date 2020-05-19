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

void Monster::set_position(DirectX::XMFLOAT4X4 pos)
{
	m_world_pos = pos;
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

DirectX::XMFLOAT4X4 Monster::get_pos()
{
	return m_world_pos;
}

char Monster::get_monster_type()
{
	return m_monster_type;
}

void Monster::move_forward(float distance)
{
	m_world_pos._41 += distance;
}
