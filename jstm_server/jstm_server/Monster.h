#pragma once
#include "header.h"

#define M_TYPE_NORMAL	1;
#define M_TYPE_MAGIC	2;
#define M_TYPE_AXE		3;
#define M_TYPE_RIDER	4;

class Monster
{
public:
	Monster();
	~Monster();
	
	void set_id(int id);
	void set_pos(DirectX::XMFLOAT4X4 pos);
	void set_monster_type(char monster_type);
	void set_monster_isLive(bool flag);

	int get_monster_id();
	DirectX::XMFLOAT4X4 get_pos();
	char get_monster_type();


private:
	int m_id;
	char m_monster_type;
	bool m_isLive = false;

	DirectX::XMFLOAT4X4 m_world_pos;
};

