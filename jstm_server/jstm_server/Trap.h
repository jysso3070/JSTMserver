#pragma once
#include "header.h"

class Trap
{
public:
	Trap();
	~Trap();

	void set_trap_pos(DirectX::XMFLOAT4X4 pos);
	void set_trap_type(char trap_type);
	DirectX::XMFLOAT4X4 get_pos();
	char get_type();

private:
	DirectX::XMFLOAT4X4 m_world_pos;
	short m_damage;
	char m_trap_type;
};

