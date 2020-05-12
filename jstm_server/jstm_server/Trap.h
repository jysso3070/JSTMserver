#pragma once
#include "header.h"

class Trap
{
public:
	Trap();
	~Trap();


	void set_trap_id(int id);
	void set_trap_pos(DirectX::XMFLOAT4X4 pos);
	void set_trap_type(char trap_type);

	int get_trap_id();
	DirectX::XMFLOAT4X4 get_pos();
	char get_type();

private:
	int m_id;
	DirectX::XMFLOAT4X4 m_world_pos;
	short m_damage;
	char m_trap_type;
};

