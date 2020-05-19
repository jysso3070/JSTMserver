#pragma once
#include "header.h"


class Monster
{
public:
	Monster();
	~Monster();
	
	void set_id(int id);
	void set_position(DirectX::XMFLOAT4X4 pos);
	void set_monster_type(char monster_type);
	void set_monster_isLive(bool flag);

	int get_monster_id();
	DirectX::XMFLOAT4X4 get_pos();
	char get_monster_type();

	void move_forward(float distance);


private:
	int m_id;
	char m_monster_type;
	bool m_isLive = false;

	DirectX::XMFLOAT4X4 m_world_pos;
};

