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

DirectX::XMFLOAT4X4 Monster::get_pos()
{
	return m_world_pos;
}
