#pragma once
#include "header.h"

class Player
{
public:
	Player();
	~Player();

private:
	int m_id;
	char m_state;
	bool damageCooltime;
	DirectX::XMFLOAT4X4 m_4x4world_pos;
	short animation_state;
	short room_number;
	short hp;
	short gold;


};

