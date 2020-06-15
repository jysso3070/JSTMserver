#pragma once
#include "header.h"

class Trap
{
public:
	Trap();
	~Trap();


	void set_trap_id(const int& id);
	void set_4x4position(const DirectX::XMFLOAT4X4& pos);
	void set_trap_type(const char& trap_type);
	void set_enable(const bool& enable_flag);

	int get_trap_id();
	DirectX::XMFLOAT4X4 get_4x4position();
	DirectX::XMFLOAT3 get_position();
	char get_type();
	bool get_enable() { return m_enable; }

private:
	bool m_enable = false;
	int m_id;
	DirectX::XMFLOAT4X4 m_4x4position;
	short m_damage;
	char m_trap_type;
};

