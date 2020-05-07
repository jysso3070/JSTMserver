#include "Monster.h"



Monster::Monster()
{
}


Monster::~Monster()
{
}

void Monster::set_pos(DirectX::XMFLOAT4X4 pos)
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

DirectX::XMFLOAT4X4 Monster::get_pos()
{
	return m_world_pos;
}

char Monster::get_mob_type()
{
	return m_monster_type;
}
