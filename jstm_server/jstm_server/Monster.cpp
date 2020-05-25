#include "Monster.h"



Monster::Monster()
{
}


Monster::~Monster()
{
}

void Monster::init()
{
	m_4x4position._14 = 0.f;
	m_4x4position._24 = 0.f;
	m_4x4position._34 = 0.f;
	m_4x4position._44 = 1.f;
}

void Monster::set_id(int id)
{
	m_id = id;
}

void Monster::set_4x4position(DirectX::XMFLOAT4X4 pos)
{
	m_4x4position = pos;
}

void Monster::set_look(XMFLOAT3 look)
{
	m_4x4position._31 = look.x;
	m_4x4position._32 = look.y;
	m_4x4position._33 = look.z;
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

bool Monster::get_isLive()
{
	return m_isLive;
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

	XMFLOAT3 look = Vector3::Subtract(get_3x3position(), target_postion);
	look = Vector3::Normalize(look);
	m_4x4position._31 = -look.x;
	m_4x4position._32 = -look.y;
	m_4x4position._33 = -look.z;

	m_4x4position._21 = 0.f;
	m_4x4position._22 = 1.f;
	m_4x4position._23 = 0.f;

	XMFLOAT3 right = Vector3::CrossProduct(get_up(), look, true);
	m_4x4position._11 = -right.x;
	m_4x4position._12 = -right.y;
	m_4x4position._13 = -right.z;
}
