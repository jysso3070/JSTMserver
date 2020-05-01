#include "Collision.h"



Collision::Collision()
{
}


Collision::~Collision()
{
}

bool Collision::distance_collision_check(DirectX::XMFLOAT4X4 o1, DirectX::XMFLOAT4X4 o2, float c_distance)
{
	float o1_x = o1._41;
	float o1_y = o1._42;
	float o1_z = o1._43;
	float o2_x = o1._41;
	float o2_y = o1._42;
	float o2_z = o1._43;
	if (sqrtf(((o1_x - o2_x)*(o1_x - o2_x)) + ((o1_y - o2_y)*(o1_y - o2_y)) + ((o1_z - o2_z)*(o1_z - o2_z))) <= c_distance) {
		return true;
	}
	else {
		return false;
	}

}

