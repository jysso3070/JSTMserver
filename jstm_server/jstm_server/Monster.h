#pragma once
#include "header.h"

class Monster
{
public:
	Monster();
	~Monster();
	
	void set_pos(DirectX::XMFLOAT4X4 pos);

	DirectX::XMFLOAT4X4 get_pos();


private:
	DirectX::XMFLOAT4X4 m_world_pos;
	bool m_isLive = false;
	char m_mob_type;
};

