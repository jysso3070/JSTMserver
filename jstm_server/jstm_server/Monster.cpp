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

void Monster::set_4x4position(const XMFLOAT4X4& pos)
{
	m_4x4position = pos;
}

void Monster::set_look(const XMFLOAT3& look)
{
	m_4x4position._31 = look.x;
	m_4x4position._32 = look.y;
	m_4x4position._33 = look.z;
}

void Monster::set_monster_type(const char& monster_type)
{
	m_monster_type = monster_type;
}

void Monster::set_monster_isLive(const bool& flag)
{
	m_isLive = flag;
}

void Monster::set_pathLine(const short& line)
{
	m_pathLine = line;
}

void Monster::set_checkPoint(const short & checkNum)
{
	m_path_checkPoint = checkNum;
}

void Monster::set_stage_number(const short & stageNum)
{
	m_stage_number = stageNum;
}

void Monster::set_position(const XMFLOAT3& position)
{
	m_4x4position._41 = position.x;
	m_4x4position._42 = position.y;
	m_4x4position._43 = position.z;
}

void Monster::set_target_id(const int & target_id)
{
	m_target_id = target_id;
}

void Monster::set_animation_state(const short & ani_state)
{
	m_animation_state = ani_state;
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


void Monster::move_forward(const float& distance)
{
	//m_4x4position._41 += distance;

	XMFLOAT3 position = get_position();
	XMFLOAT3 look = get_look();
	position = Vector3::Add(position, look, distance);
	set_position(position);

	m_4x4position._42 = -50.f;
}

void Monster::set_aggro_direction(DirectX::XMFLOAT3 target_postion)
{
	target_postion.y = -50.f;

	XMFLOAT3 look = Vector3::Subtract(get_position(), target_postion);
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
