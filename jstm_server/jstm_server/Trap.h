#pragma once
#include "header.h"

class Trap
{
public:
	Trap();
	~Trap();


	void set_trap_id(const unsigned short& id);
	void set_4x4position(const DirectX::XMFLOAT4X4& pos);
	void set_trap_type(const char& trap_type);
	void set_enable(const bool& enable_flag);
	void set_trapPos(const DirectX::XMFLOAT3 pos) { 
		m_4x4position._41 = pos.x;
		m_4x4position._42 = pos.y;
		m_4x4position._43 = pos.z;
	}
	void set_wallDir(unsigned char wallDir);
	void set_wallTrapOn(const bool& flag);

	unsigned short get_trap_id();
	DirectX::XMFLOAT4X4 get_4x4position();
	DirectX::XMFLOAT3 get_position();
	char get_type();
	bool get_enable() { return m_enable; }
	unsigned char get_wallDir() { return m_wallDir; }
	bool get_wallTrapOn() { return m_wallTrapOn; }

private:
	volatile bool m_enable;
	volatile bool m_wallTrapOn;
	unsigned short m_id;
	DirectX::XMFLOAT4X4 m_4x4position;
	short m_damage;
	char m_trap_type;
	unsigned char m_wallDir;

};

