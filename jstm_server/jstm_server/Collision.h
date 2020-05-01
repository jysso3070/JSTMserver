#pragma once
#include <DirectXMath.h>
#include <math.h>

class Collision
{
public:
	Collision();
	~Collision();

public:
	bool distance_collision_check(DirectX::XMFLOAT4X4 o1, DirectX::XMFLOAT4X4 o2, float distance);
};

