#include "Trap.h"


Trap::Trap()
{
	m_enable = false;
}


Trap::~Trap()
{
	delete this;
}

void Trap::set_trap_id(const unsigned short& id)
{
	m_id = id;
}

void Trap::set_4x4position(const DirectX::XMFLOAT4X4& pos)
{
	m_4x4position = pos;
}

void Trap::set_trap_type(const char& trap_type)
{
	m_trap_type = trap_type;
}

void Trap::set_enable(const bool & enable_flag)
{
	m_enable = enable_flag;
}

void Trap::set_wallDir(unsigned char wallDir)
{
	m_wallDir = wallDir;
}

void Trap::set_wallTrapOn(const bool & flag)
{
	m_wallTrapOn = flag;
}

unsigned short Trap::get_trap_id()
{
	return m_id;
}

DirectX::XMFLOAT4X4 Trap::get_4x4position()
{
	return m_4x4position;
}

DirectX::XMFLOAT3 Trap::get_position()
{
	return DirectX::XMFLOAT3(m_4x4position._41, m_4x4position._42, m_4x4position._43);
}

char Trap::get_type()
{
	return m_trap_type;
}
