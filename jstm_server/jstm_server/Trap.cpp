#include "Trap.h"


Trap::Trap()
{
}


Trap::~Trap()
{
	delete this;
}

void Trap::set_trap_id(int id)
{
	m_id = id;
}

void Trap::set_trap_pos(DirectX::XMFLOAT4X4 pos)
{
	m_world_pos = pos;
}

void Trap::set_trap_type(char trap_type)
{
	m_trap_type = trap_type;
}

int Trap::get_trap_id()
{
	return m_id;
}

DirectX::XMFLOAT4X4 Trap::get_pos()
{
	return m_world_pos;
}

char Trap::get_type()
{
	return m_trap_type;
}
