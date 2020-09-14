#include "Monster.h"



Monster::Monster()
{
	m_buffType = TRAP_BUFF_NONE;
	m_isLive = false;
	m_checkPoint_1 = XMFLOAT3(0.f, 0.f, 0.f);
	m_checkPoint_2 = XMFLOAT3(0.f, 0.f, 0.f);
	m_checkPoint_3 = XMFLOAT3(0.f, 0.f, 0.f);
	m_checkPoint_4 = XMFLOAT3(0.f, 0.f, 0.f);
	m_checkPoint_5 = XMFLOAT3(0.f, 0.f, 0.f);
}


Monster::~Monster()
{
}

void Monster::init()
{
	m_4x4position._14 = 0.f;
	m_4x4position._24 = 0.f;
	m_4x4position._34 = 0.f;
	m_4x4position._44 = 1.f;
}

void Monster::set_id(short id)
{
	m_id = id;
}

void Monster::set_4x4position(const XMFLOAT4X4& pos)
{
	m_4x4position = pos;
}

void Monster::set_look(const XMFLOAT3& look)
{
	m_4x4position._31 = look.x;
	m_4x4position._32 = look.y;
	m_4x4position._33 = look.z;
}

void Monster::set_monster_type(const char& monster_type)
{
	m_monster_type = monster_type;
	if (monster_type == TYPE_ORC) { this->m_hp = ORC_HP; }
	else if (monster_type == TYPE_SHAMAN) { this->m_hp = SHAMAN_HP; }
	else if (monster_type == TYPE_STRONGORC) { this->m_hp = STRONGORC_HP; }
	else if (monster_type == TYPE_RIDER) { this->m_hp = RIDER_HP; }
}

void Monster::set_isLive(const bool& flag)
{
	m_isLive = flag;
}

void Monster::set_pathLine(const short& line)
{
	m_pathLine = line;
}

void Monster::set_checkPoint(const short & checkNum)
{
	m_path_checkPoint = checkNum;
}

void Monster::set_stage_number(const short & stageNum)
{
	m_stage_number = stageNum;
}

void Monster::set_position(const XMFLOAT3& position)
{
	m_4x4position._41 = position.x;
	m_4x4position._42 = position.y;
	m_4x4position._43 = position.z;
}

void Monster::set_target_id(const int & target_id)
{
	m_target_id = target_id;
}

void Monster::set_animation_state(const short & ani_state)
{
	m_animation_state = ani_state;
}

void Monster::set_trap_cooltime(const bool & flag)
{
	m_trap_cooltime = flag;
}

void Monster::set_HP(const short & HP)
{
	m_hp = HP;
}

void Monster::set_arrivePortal(const bool & flag)
{
	m_arrive_portal = flag;
}

void Monster::set_attackCooltime(const bool & flag)
{
	m_attack_coolTime = flag;
}

void Monster::set_buffType(const char & buffType)
{
	m_buffType = buffType;
}

void Monster::set_bulletAnim(const bool & flag)
{
	m_bulletAnimation = flag;
}

void Monster::add_xPos(const float & dx)
{
	m_4x4position._41 += dx;
}

void Monster::add_zPos(const float & dz)
{
	m_4x4position._43 += dz;
}

short Monster::get_monster_id()
{
	return m_id;
}

char Monster::get_monster_type()
{
	return m_monster_type;
}

bool Monster::get_isLive()
{
	return m_isLive;
}

DirectX::XMFLOAT4X4 Monster::get_4x4position()
{
	return m_4x4position;
}


void Monster::gen_sequence(const short & stage_number, const short & pathLine, XMFLOAT3 position)
{
	m_arrive_portal = false;
	m_stage_number = stage_number;
	m_pathLine = pathLine;
	m_path_checkPoint = 0;
	m_animation_state = M_ANIM_RUN;
	this->make_checkPoint();
	this->set_position(position);
	this->set_isLive(true);
}

void Monster::decrease_hp(const short & damage)
{
	m_hp -= damage;
}

void Monster::move_forward(const float& distance)
{
	//m_4x4position._41 += distance;
	float temp_distance = distance;
	if (this->m_buffType == TRAP_BUFF_SLOW) {
		temp_distance /= 2;
	}

	XMFLOAT3 position = get_position();
	XMFLOAT3 look = get_look();
	position = Vector3::Add(position, look, temp_distance);
	set_position(position);

	m_4x4position._42 = -50.f;
}

void Monster::move_forward(const float & distance, Monster * monsterPool)
{
	float temp_distance = distance;
	if (this->m_monster_type == TYPE_RIDER) {
		temp_distance *= 1.2;
	}
	if (this->m_buffType == TRAP_BUFF_SLOW) {
		temp_distance /= 2;
	}

	XMFLOAT3 position = get_position();
	XMFLOAT3 look = get_look();
	position = Vector3::Add(position, look, temp_distance);
	bool collision = false;
	for (short i = 0; i < MAX_MONSTER; ++i) {
		if (monsterPool[i].get_isLive() == false) { continue; }
		if (i == m_id) { continue; }
		if (Vector3::Distance(position, monsterPool[i].get_position()) <= ORC_COLLISION_RANGE ) {
			collision = true;
			if (position.z >= monsterPool[i].get_position().z) {
				position.z += ORC_COLLISION_REBOUND;
				monsterPool[i].add_zPos(-ORC_COLLISION_REBOUND);
			}
			else {
				position.z -= ORC_COLLISION_REBOUND;
				monsterPool[i].add_zPos(ORC_COLLISION_REBOUND);
			}
			if (position.x >= monsterPool[i].get_position().x) {
				position.x += ORC_COLLISION_REBOUND;
				monsterPool[i].add_xPos(-ORC_COLLISION_REBOUND);
			}
			else {
				position.x -= ORC_COLLISION_REBOUND;
				monsterPool[i].add_xPos(ORC_COLLISION_REBOUND);
			}

			break;
		}
	}
	if(collision == false){
		set_position(position);
		m_4x4position._42 = -50.f;
	}
	else {
		set_position(position);
		m_4x4position._42 = -50.f;
	}
}

void Monster::set_aggro_direction(DirectX::XMFLOAT3 target_postion)
{
	target_postion.y = -50.f;

	XMFLOAT3 look = Vector3::Subtract(get_position(), target_postion);
	look = Vector3::Normalize(look);
	m_4x4position._31 = -look.x;
	m_4x4position._32 = -look.y;
	m_4x4position._33 = -look.z;

	m_4x4position._21 = 0.f;
	m_4x4position._22 = 1.f;
	m_4x4position._23 = 0.f;

	XMFLOAT3 right = Vector3::CrossProduct(get_up(), look, true);
	m_4x4position._11 = -right.x;
	m_4x4position._12 = -right.y;
	m_4x4position._13 = -right.z;
}

void Monster::set_rotate(float Pitch, float Yaw, float Roll)
{
	XMMATRIX Rotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(Pitch), XMConvertToRadians(Yaw), XMConvertToRadians(Roll));
	m_4x4position = Matrix4x4::Multiply(Rotate, m_4x4position);
}

void Monster::make_checkPoint()
{
	if (m_stage_number == 1) {
		if (m_pathLine == 1 || m_pathLine == 2 || m_pathLine == 3) {
			m_checkPoint_1 = XMFLOAT3(-630.f + stage1_check1(dre), -50.f, 3010.f + stage1_check1(dre));
			m_checkPoint_2 = XMFLOAT3(-120.f + stage1_check2(dre), -50.f, 2800.f + stage1_check2(dre));
			m_checkPoint_3 = XMFLOAT3(-120.f + stage1_check3(dre), -50.f, 50.f + stage1_check3_z(dre));
			m_checkPoint_4 = stage1_line1_3;
		}
		else if (m_pathLine == 4 || m_pathLine == 5 || m_pathLine == 6) {
			m_checkPoint_1 = XMFLOAT3(430.f + stage1_check1(dre), -50.f, 3030.f + stage1_check1(dre));
			m_checkPoint_2 = XMFLOAT3(0.f + stage1_check2(dre), -50.f, 2900.f + stage1_check2(dre));
			m_checkPoint_3 = XMFLOAT3(0.f + stage1_check3(dre), -50.f, 50.f + stage1_check3_z(dre));
			m_checkPoint_4 = stage1_line1_3;
		}
	}
	else if (m_stage_number == 2) {
		if (m_pathLine == 1 || m_pathLine == 2 || m_pathLine == 3) {
			m_checkPoint_1 = XMFLOAT3(-630.f + stage2_check1(dre), -50.f, 1700.f + stage2_check1(dre));
			m_checkPoint_2 = XMFLOAT3(-220.f + stage2_check2(dre), -50.f, 1520.f + stage2_check2(dre));
			m_checkPoint_3 = XMFLOAT3(-220.f + stage2_check3(dre), -50.f, 70.f + stage2_check3(dre));
			m_checkPoint_4 = stage2_4;
		}
		else if (m_pathLine == 4 || m_pathLine == 5 || m_pathLine == 6) {
			m_checkPoint_1 = XMFLOAT3(430.f + stage2_check1(dre), -50.f, 1700.f + stage2_check1(dre));
			m_checkPoint_2 = XMFLOAT3(0.f + stage2_check2(dre), -50.f, 1520.f + stage2_check2(dre));
			m_checkPoint_3 = XMFLOAT3(-220.f + stage2_check3(dre), -50.f, 70.f + stage2_check3(dre));
			m_checkPoint_4 = stage2_4;
		}
		else if (m_pathLine == 7 || m_pathLine == 8 || m_pathLine == 9) {
			m_checkPoint_1 = XMFLOAT3(-630.f + stage2_check1(dre), -50.f, -1870.f + stage2_check1(dre));
			m_checkPoint_2 = XMFLOAT3(-220.f + stage2_check2(dre), -50.f, -1640.f + stage2_check2(dre));
			m_checkPoint_3 = XMFLOAT3(-220.f + stage2_check3(dre), -50.f, 70.f - stage2_check3(dre));
			m_checkPoint_4 = stage2_4;
		}
		else if (m_pathLine == 10 || m_pathLine == 11 || m_pathLine == 12) {
			m_checkPoint_1 = XMFLOAT3(430.f + stage2_check1(dre), -50.f, -1870.f + stage2_check1(dre));
			m_checkPoint_2 = XMFLOAT3(0.f + stage2_check2(dre), -50.f, -1640.f + stage2_check2(dre));
			m_checkPoint_3 = XMFLOAT3(-220.f + stage2_check3(dre), -50.f, 70.f - stage2_check3(dre));
			m_checkPoint_4 = stage2_4;
		}
	}
	else if (m_stage_number == 3) {
		if (m_pathLine == 1 || m_pathLine == 2 || m_pathLine == 3) {
			m_checkPoint_1 = XMFLOAT3(-1260.f + stage3_check1(dre), -50.f, 1570.f + stage3_check1(dre));
			m_checkPoint_2 = XMFLOAT3(-1070.f + stage3_check2(dre), -50.f, -950.f + stage3_check2(dre));
			m_checkPoint_3 = XMFLOAT3(-220.f + stage3_check3(dre), -50.f, -900.f + stage3_check3(dre));
			m_checkPoint_4 = XMFLOAT3(-250.f + stage3_check4(dre), -50.f, 0.f - stage3_check4(dre));
			m_checkPoint_5 = stage3_5;
		}
		else if (m_pathLine == 4 || m_pathLine == 5 || m_pathLine == 6) {
			m_checkPoint_1 = XMFLOAT3(-1100.f + stage3_check1(dre), -50.f, 1570.f + stage3_check1(dre));
			m_checkPoint_2 = XMFLOAT3(-1070.f + stage3_check2(dre), -50.f, -950.f + stage3_check2(dre));
			m_checkPoint_3 = XMFLOAT3(-220.f + stage3_check3(dre), -50.f, -900.f + stage3_check3(dre));
			m_checkPoint_4 = XMFLOAT3(-250.f + stage3_check4(dre), -50.f, 0.f - stage3_check4(dre));
			m_checkPoint_5 = stage3_5;
		}
		else if (m_pathLine == 7 || m_pathLine == 8 || m_pathLine == 9) {
			m_checkPoint_1 = XMFLOAT3(1050.f + stage3_check1(dre), -50.f, -1800.f + stage3_check1(dre));
			m_checkPoint_2 = XMFLOAT3(870.f + stage3_check2(dre), -50.f, 740.f + stage3_check2(dre));
			m_checkPoint_3 = XMFLOAT3(0.f + stage3_check3(dre), -50.f, 740.f + stage3_check3(dre));
			m_checkPoint_4 = XMFLOAT3(-250.f + stage3_check4(dre), -50.f, 0.f + stage3_check4(dre));
			m_checkPoint_5 = stage3_5;
		}
		else if (m_pathLine == 10 || m_pathLine == 11 || m_pathLine == 12) {
			m_checkPoint_1 = XMFLOAT3(900.f + stage3_check1(dre), -50.f, -1800.f + stage3_check1(dre));
			m_checkPoint_2 = XMFLOAT3(870.f + stage3_check2(dre), -50.f, 740.f + stage3_check2(dre));
			m_checkPoint_3 = XMFLOAT3(0.f + stage3_check3(dre), -50.f, 740.f + stage3_check3(dre));
			m_checkPoint_4 = XMFLOAT3(-250.f + stage3_check4(dre), -50.f, 0.f + stage3_check4(dre));
			m_checkPoint_5 = stage3_5;
		}
	}
}

void Monster::process_move_path_t()
{
	if (m_stage_number == 1) { // stage 1
		if (m_path_checkPoint == 0) {
			set_aggro_direction(m_checkPoint_1);
			if (Vector3::Distance(this->get_position(), m_checkPoint_1) <= PATH_CHECKPOINT_RANGE) {
				this->set_checkPoint(1);
			}
		}
		else if (m_path_checkPoint == 1) {
			set_aggro_direction(m_checkPoint_2);
			if (Vector3::Distance(this->get_position(), m_checkPoint_2) <= PATH_CHECKPOINT_RANGE) {
				this->set_checkPoint(2);
			}
		}
		else if (m_path_checkPoint == 2) {
			set_aggro_direction(m_checkPoint_3);
			if (Vector3::Distance(this->get_position(), m_checkPoint_3) <= PATH_CHECKPOINT_RANGE) {
				this->set_checkPoint(3);
			}
		}
		else if (m_path_checkPoint == 3) {
			set_aggro_direction(m_checkPoint_4);
			if (Vector3::Distance(this->get_position(), m_checkPoint_4) <= PATH_CHECKPOINT_RANGE) {
				std::cout << "Æ÷Å» µµÂø" << endl;
				this->m_arrive_portal = true;
			}
		}
		//this->move_forward(5.f);
		this->set_animation_state(M_ANIM_RUN);
	}
	else if (m_stage_number == 2) { // stage 2
		if (m_path_checkPoint == 0) {
			set_aggro_direction(m_checkPoint_1);
			if (Vector3::Distance(this->get_position(), m_checkPoint_1) <= PATH_CHECKPOINT_RANGE) {
				this->set_checkPoint(1);
			}
		}
		else if (m_path_checkPoint == 1) {
			set_aggro_direction(m_checkPoint_2);
			if (Vector3::Distance(this->get_position(), m_checkPoint_2) <= PATH_CHECKPOINT_RANGE) {
				this->set_checkPoint(2);
			}
		}
		else if (m_path_checkPoint == 2) {
			set_aggro_direction(m_checkPoint_3);
			if (Vector3::Distance(this->get_position(), m_checkPoint_3) <= PATH_CHECKPOINT_RANGE) {
				this->set_checkPoint(3);
			}
		}
		else if (m_path_checkPoint == 3) {
			set_aggro_direction(m_checkPoint_4);
			if (Vector3::Distance(this->get_position(), m_checkPoint_4) <= PATH_CHECKPOINT_RANGE) {
				std::cout << "Æ÷Å» µµÂø" << endl;
				this->m_arrive_portal = true;
			}
		}
		//this->move_forward(5.f);
		this->set_animation_state(M_ANIM_RUN);
	}
	else if (m_stage_number == 3) { // stage 3
		if (m_path_checkPoint == 0) {
			set_aggro_direction(m_checkPoint_1);
			if (Vector3::Distance(this->get_position(), m_checkPoint_1) <= PATH_CHECKPOINT_RANGE) {
				this->set_checkPoint(1);
			}
		}
		else if (m_path_checkPoint == 1) {
			set_aggro_direction(m_checkPoint_2);
			if (Vector3::Distance(this->get_position(), m_checkPoint_2) <= PATH_CHECKPOINT_RANGE) {
				this->set_checkPoint(2);
			}
		}
		else if (m_path_checkPoint == 2) {
			set_aggro_direction(m_checkPoint_3);
			if (Vector3::Distance(this->get_position(), m_checkPoint_3) <= PATH_CHECKPOINT_RANGE) {
				this->set_checkPoint(3);
			}
		}
		else if (m_path_checkPoint == 3) {
			set_aggro_direction(m_checkPoint_4);
			if (Vector3::Distance(this->get_position(), m_checkPoint_4) <= PATH_CHECKPOINT_RANGE) {
				this->set_checkPoint(4);
			}
		}
		else if (m_path_checkPoint == 4) {
			set_aggro_direction(m_checkPoint_5);
			if (Vector3::Distance(this->get_position(), m_checkPoint_5) <= PATH_CHECKPOINT_RANGE) {
				std::cout << "Æ÷Å» µµÂø" << endl;
				this->m_arrive_portal = true;
			}
		}
		//this->move_forward(5.f);
		this->set_animation_state(M_ANIM_RUN);
	}

}

void Monster::aggro_release()
{
	if (this->m_stage_number == 1) {
		if (this->get_position().x > -770.f && this->get_position().z < 4300.f &&
			this->get_position().x <= -330.f && this->get_position().z >= 3340.f) {
			this->set_checkPoint(0);
		}
		else if (this->get_position().x < 770.f && this->get_position().z < 4300.f &&
			this->get_position().x >= 330.f && this->get_position().z >= 3340.f) {
			this->set_checkPoint(0);
		}
		else if (this->get_position().x > -770.f && this->get_position().z < 3340.f &&
			this->get_position().x <= -330.f && this->get_position().z > 2830.f) {
			this->set_checkPoint(1);
		}
		else if (this->get_position().x < 770.f && this->get_position().z < 3340.f &&
			this->get_position().x >= 330.f && this->get_position().z > 2830.f) {
			this->set_checkPoint(1);
		}
		else if (this->get_position().x > -330.f && this->get_position().z < 3340.f &&
			this->get_position().x < 330.f && this->get_position().z > 150.f) {
			this->set_checkPoint(2);
		}
		return;
	}
	else if (this->m_stage_number == 2) {
		if (this->get_position().x > -780.f && this->get_position().z < 2890.f &&
			this->get_position().x <= -330.f && this->get_position().z >= 1990.f) {
			this->set_checkPoint(0);
		}
		else if (this->get_position().x < 780.f && this->get_position().z < 2890.f &&
			this->get_position().x >= 330.f && this->get_position().z >= 1990.f) {
			this->set_checkPoint(0);
		}
		else if (this->get_position().x > -780.f && this->get_position().z < 1990.f &&
			this->get_position().x <= -330.f && this->get_position().z > 1470.f) {
			this->set_checkPoint(1);
		}
		else if (this->get_position().x < 780.f && this->get_position().z < 1990.f &&
			this->get_position().x >= 330.f && this->get_position().z > 1470.f) {
			this->set_checkPoint(1);
		}
		else if (this->get_position().x > -330.f && this->get_position().z < 1990.f &&
			this->get_position().x < 330.f && this->get_position().z > 150.f) {
			this->set_checkPoint(2);
		}
		else if (this->get_position().x > -330.f && this->get_position().z <= 150.f &&
			this->get_position().x < 330.f && this->get_position().z >= 0.f) {
			this->set_checkPoint(3);
		} // ---------------------------------------------------------------------------¿©±îÁö À§
		else if (this->get_position().x > -780.f && this->get_position().z > -2890.f &&
			this->get_position().x <= -330.f && this->get_position().z <= -1990.f) {
			this->set_checkPoint(0);
		}
		else if (this->get_position().x < 780.f && this->get_position().z > -2890.f &&
			this->get_position().x >= 330.f && this->get_position().z <= -1990.f) {
			this->set_checkPoint(0);
		}
		else if (this->get_position().x > -780.f && this->get_position().z > -1990.f &&
			this->get_position().x <= -330.f && this->get_position().z < -1470.f) {
			this->set_checkPoint(1);
		}
		else if (this->get_position().x < 780.f && this->get_position().z > -1990.f &&
			this->get_position().x >= 330.f && this->get_position().z < -1470.f) {
			this->set_checkPoint(1);
		}
		else if (this->get_position().x > -330.f && this->get_position().z > -1990.f &&
			this->get_position().x < 330.f && this->get_position().z < -150.f) {
			this->set_checkPoint(2);
		}
		else if (this->get_position().x > -330.f && this->get_position().z >= -150.f &&
			this->get_position().x < 330.f && this->get_position().z <= 0.f) {
			this->set_checkPoint(3);
		} //-----------------------------------------------------------------------

		return;
	}
	else if (this->m_stage_number == 3) {
		if (this->get_position().x > -1950.f && this->get_position().z < 1950.f &&
			this->get_position().x <= -1330.f && this->get_position().z >= 1500.f) {
			this->set_checkPoint(0);
		}
		else if (this->get_position().x < 640.f && this->get_position().z < 1950.f &&
			this->get_position().x >= -810.f && this->get_position().z >= 1500.f) {
			this->set_checkPoint(0);
		}
		else if (this->get_position().x > -1330.f && this->get_position().z < 1950.f &&
			this->get_position().x < -810.f && this->get_position().z >= -670.f) {
			this->set_checkPoint(1);
		}
		else if (this->get_position().x > -1360.f && this->get_position().z < -670.f &&
			this->get_position().x <= -330.f && this->get_position().z >= -1080.f) {
			this->set_checkPoint(2);
		}
		else if (this->get_position().x > -330.f && this->get_position().z < -150.f &&
			this->get_position().x <= 350.f && this->get_position().z >= -1080.f) {
			this->set_checkPoint(3);
		}
		else if (this->get_position().x > -330.f && this->get_position().z <= 0.f &&
			this->get_position().x <= 350.f && this->get_position().z >= -150.f) {
			this->set_checkPoint(4);
		} // ------------------------------------------------------------------------- ¿©±â±îÁö À§
		else if (this->get_position().x < 1950.f && this->get_position().z > -1950.f &&
			this->get_position().x >= 1330.f && this->get_position().z <= -1500.f) {
			this->set_checkPoint(0);
		}
		else if (this->get_position().x <= 810.f && this->get_position().z > -1950.f &&
			this->get_position().x > -640.f && this->get_position().z <= -1500.f) {
			this->set_checkPoint(0);
		}
		else if (this->get_position().x > 810.f && this->get_position().z > -1950.f &&
			this->get_position().x < 1330.f && this->get_position().z <= 670.f) {
			this->set_checkPoint(1);
		}
		else if (this->get_position().x < 1360.f && this->get_position().z > 670.f &&
			this->get_position().x >= 330.f && this->get_position().z < 1080.f) {
			this->set_checkPoint(2);
		}
		else if (this->get_position().x < 330.f && this->get_position().z > 150.f &&
			this->get_position().x > -350.f && this->get_position().z < 1080.f) {
			this->set_checkPoint(3);
		}
		else if (this->get_position().x < 330.f && this->get_position().z >= 0.f &&
			this->get_position().x > -350.f && this->get_position().z <= 150.f) {
			this->set_checkPoint(3);
		}
		return;
	}
}

void Monster::process_move_path()
{
	if (m_stage_number == 1) { // stage 1
		if (m_pathLine == 1) { // stage 1 path 1
			if (m_path_checkPoint == 0) {
				set_aggro_direction(m_checkPoint_1);
				if (Vector3::Distance(this->get_position(), m_checkPoint_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(m_checkPoint_2);
				if (Vector3::Distance(this->get_position(), m_checkPoint_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(m_checkPoint_3);
				if (Vector3::Distance(this->get_position(), m_checkPoint_3) <= PATH_CHECKPOINT_RANGE) {
					std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 2) { // stage 1 path 2
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage1_line2_1);
				if (Vector3::Distance(this->get_position(), stage1_line2_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage1_line2_2);
				if (Vector3::Distance(this->get_position(), stage1_line2_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage1_line2_3);
				if (Vector3::Distance(this->get_position(), stage1_line2_3) <= PATH_CHECKPOINT_RANGE) {
					std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 3) { // stage 1 path 3
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage1_line3_1);
				if (Vector3::Distance(this->get_position(), stage1_line3_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage1_line3_2);
				if (Vector3::Distance(this->get_position(), stage1_line3_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage1_line3_3);
				if (Vector3::Distance(this->get_position(), stage1_line3_3) <= PATH_CHECKPOINT_RANGE) {
					std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 4) { // stage 1 path 4
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage1_line4_1);
				if (Vector3::Distance(this->get_position(), stage1_line4_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage1_line4_2);
				if (Vector3::Distance(this->get_position(), stage1_line4_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage1_line4_3);
				if (Vector3::Distance(this->get_position(), stage1_line4_3) <= PATH_CHECKPOINT_RANGE) {
					std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 5) { // stage 1 path 5
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage1_line5_1);
				if (Vector3::Distance(this->get_position(), stage1_line5_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage1_line5_2);
				if (Vector3::Distance(this->get_position(), stage1_line5_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage1_line5_3);
				if (Vector3::Distance(this->get_position(), stage1_line5_3) <= PATH_CHECKPOINT_RANGE) {
					std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 6) { // stage 1 path 6
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage1_line6_1);
				if (Vector3::Distance(this->get_position(), stage1_line6_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage1_line6_2);
				if (Vector3::Distance(this->get_position(), stage1_line6_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage1_line6_3);
				if (Vector3::Distance(this->get_position(), stage1_line6_3) <= PATH_CHECKPOINT_RANGE) {
					std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
	}
	// -------------stage 2--------------------------------------------------------
	else if (m_stage_number == 2) {
		if (m_pathLine == 1) { // stage 2 path 1
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage2_line1_1);
				if (Vector3::Distance(this->get_position(), stage2_line1_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage2_line1_2);
				if (Vector3::Distance(this->get_position(), stage2_line1_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage2_line1_3);
				if (Vector3::Distance(this->get_position(), stage2_line1_3) <= PATH_CHECKPOINT_RANGE) {
					//std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 2) { // stage 2 path 2
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage2_line2_1);
				if (Vector3::Distance(this->get_position(), stage2_line2_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage2_line2_2);
				if (Vector3::Distance(this->get_position(), stage2_line2_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage2_line2_3);
				if (Vector3::Distance(this->get_position(), stage2_line2_3) <= PATH_CHECKPOINT_RANGE) {
					//std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 3) { // stage 2 path 3
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage2_line3_1);
				if (Vector3::Distance(this->get_position(), stage2_line3_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage2_line3_2);
				if (Vector3::Distance(this->get_position(), stage2_line3_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage2_line3_3);
				if (Vector3::Distance(this->get_position(), stage2_line3_3) <= PATH_CHECKPOINT_RANGE) {
					//std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 4) { // stage 2 path 4
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage2_line4_1);
				if (Vector3::Distance(this->get_position(), stage2_line4_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage2_line4_2);
				if (Vector3::Distance(this->get_position(), stage2_line4_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage2_line4_3);
				if (Vector3::Distance(this->get_position(), stage2_line4_3) <= PATH_CHECKPOINT_RANGE) {
					//std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 5) { // stage 2 path 5
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage2_line5_1);
				if (Vector3::Distance(this->get_position(), stage2_line5_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage2_line5_2);
				if (Vector3::Distance(this->get_position(), stage2_line5_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage2_line5_3);
				if (Vector3::Distance(this->get_position(), stage2_line5_3) <= PATH_CHECKPOINT_RANGE) {
					//std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 6) { // stage 2 path 6
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage2_line6_1);
				if (Vector3::Distance(this->get_position(), stage2_line6_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage2_line6_2);
				if (Vector3::Distance(this->get_position(), stage2_line6_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage2_line6_3);
				if (Vector3::Distance(this->get_position(), stage2_line6_3) <= PATH_CHECKPOINT_RANGE) {
					//std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 7) { // stage 2 path 7
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage2_line7_1);
				if (Vector3::Distance(this->get_position(), stage2_line7_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage2_line7_2);
				if (Vector3::Distance(this->get_position(), stage2_line7_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage2_line7_3);
				if (Vector3::Distance(this->get_position(), stage2_line7_3) <= PATH_CHECKPOINT_RANGE) {
					//std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 8) { // stage 2 path 8
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage2_line8_1);
				if (Vector3::Distance(this->get_position(), stage2_line8_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage2_line8_2);
				if (Vector3::Distance(this->get_position(), stage2_line8_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage2_line8_3);
				if (Vector3::Distance(this->get_position(), stage2_line8_3) <= PATH_CHECKPOINT_RANGE) {
					//std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 9) { // stage 2 path 9
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage2_line9_1);
				if (Vector3::Distance(this->get_position(), stage2_line9_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage2_line9_2);
				if (Vector3::Distance(this->get_position(), stage2_line9_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage2_line9_3);
				if (Vector3::Distance(this->get_position(), stage2_line9_3) <= PATH_CHECKPOINT_RANGE) {
					//std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 10) { // stage 2 path 10
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage2_line10_1);
				if (Vector3::Distance(this->get_position(), stage2_line10_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage2_line10_2);
				if (Vector3::Distance(this->get_position(), stage2_line10_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage2_line10_3);
				if (Vector3::Distance(this->get_position(), stage2_line10_3) <= PATH_CHECKPOINT_RANGE) {
					//std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 11) { // stage 2 path 11
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage2_line11_1);
				if (Vector3::Distance(this->get_position(), stage2_line11_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage2_line11_2);
				if (Vector3::Distance(this->get_position(), stage2_line11_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage2_line11_3);
				if (Vector3::Distance(this->get_position(), stage2_line11_3) <= PATH_CHECKPOINT_RANGE) {
					//std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 12) { // stage 2 path 12
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage2_line12_1);
				if (Vector3::Distance(this->get_position(), stage2_line12_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage2_line12_2);
				if (Vector3::Distance(this->get_position(), stage2_line12_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage2_line12_3);
				if (Vector3::Distance(this->get_position(), stage2_line12_3) <= PATH_CHECKPOINT_RANGE) {
					//std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
	}
	// -------------stage 3--------------------------------------------------------
	else if (m_stage_number == 3) { // stage 3
		if (m_pathLine == 1) { // stage 3 path 1
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage3_line1_1);
				if (Vector3::Distance(this->get_position(), stage3_line1_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage3_line1_2);
				if (Vector3::Distance(this->get_position(), stage3_line1_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage3_line1_3);
				if (Vector3::Distance(this->get_position(), stage3_line1_3) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(3);
				}
			}
			else if (m_path_checkPoint == 3) {
				set_aggro_direction(stage3_line1_4);
				if (Vector3::Distance(this->get_position(), stage3_line1_4) <= PATH_CHECKPOINT_RANGE) {
					std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 2) { // stage 3 path 2
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage3_line2_1);
				if (Vector3::Distance(this->get_position(), stage3_line2_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage3_line2_2);
				if (Vector3::Distance(this->get_position(), stage3_line2_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage3_line2_3);
				if (Vector3::Distance(this->get_position(), stage3_line2_3) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(3);
				}
			}
			else if (m_path_checkPoint == 3) {
				set_aggro_direction(stage3_line2_4);
				if (Vector3::Distance(this->get_position(), stage3_line2_4) <= PATH_CHECKPOINT_RANGE) {
					std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 3) { // stage 3 path 3
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage3_line3_1);
				if (Vector3::Distance(this->get_position(), stage3_line3_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage3_line3_2);
				if (Vector3::Distance(this->get_position(), stage3_line3_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage3_line3_3);
				if (Vector3::Distance(this->get_position(), stage3_line3_3) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(3);
				}
			}
			else if (m_path_checkPoint == 3) {
				set_aggro_direction(stage3_line3_4);
				if (Vector3::Distance(this->get_position(), stage3_line3_4) <= PATH_CHECKPOINT_RANGE) {
					std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 4) { // stage 3 path 4
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage3_line4_1);
				if (Vector3::Distance(this->get_position(), stage3_line4_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage3_line4_2);
				if (Vector3::Distance(this->get_position(), stage3_line4_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage3_line4_3);
				if (Vector3::Distance(this->get_position(), stage3_line4_3) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(3);
				}
			}
			else if (m_path_checkPoint == 3) {
				set_aggro_direction(stage3_line4_4);
				if (Vector3::Distance(this->get_position(), stage3_line4_4) <= PATH_CHECKPOINT_RANGE) {
					std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 5) { // stage 3 path 5
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage3_line5_1);
				if (Vector3::Distance(this->get_position(), stage3_line5_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage3_line5_2);
				if (Vector3::Distance(this->get_position(), stage3_line5_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage3_line5_3);
				if (Vector3::Distance(this->get_position(), stage3_line5_3) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(3);
				}
			}
			else if (m_path_checkPoint == 3) {
				set_aggro_direction(stage3_line5_4);
				if (Vector3::Distance(this->get_position(), stage3_line5_4) <= PATH_CHECKPOINT_RANGE) {
					std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 6) { // stage 3 path 6
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage3_line6_1);
				if (Vector3::Distance(this->get_position(), stage3_line6_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage3_line6_2);
				if (Vector3::Distance(this->get_position(), stage3_line6_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage3_line6_3);
				if (Vector3::Distance(this->get_position(), stage3_line6_3) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(3);
				}
			}
			else if (m_path_checkPoint == 3) {
				set_aggro_direction(stage3_line6_4);
				if (Vector3::Distance(this->get_position(), stage3_line6_4) <= PATH_CHECKPOINT_RANGE) {
					std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 7) { // stage 3 path 7
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage3_line7_1);
				if (Vector3::Distance(this->get_position(), stage3_line7_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage3_line7_2);
				if (Vector3::Distance(this->get_position(), stage3_line7_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage3_line7_3);
				if (Vector3::Distance(this->get_position(), stage3_line7_3) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(3);
				}
			}
			else if (m_path_checkPoint == 3) {
				set_aggro_direction(stage3_line7_4);
				if (Vector3::Distance(this->get_position(), stage3_line7_4) <= PATH_CHECKPOINT_RANGE) {
					std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 8) { // stage 3 path 8
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage3_line8_1);
				if (Vector3::Distance(this->get_position(), stage3_line8_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage3_line8_2);
				if (Vector3::Distance(this->get_position(), stage3_line8_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage3_line8_3);
				if (Vector3::Distance(this->get_position(), stage3_line8_3) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(3);
				}
			}
			else if (m_path_checkPoint == 3) {
				set_aggro_direction(stage3_line8_4);
				if (Vector3::Distance(this->get_position(), stage3_line8_4) <= PATH_CHECKPOINT_RANGE) {
					std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 9) { // stage 3 path 9
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage3_line9_1);
				if (Vector3::Distance(this->get_position(), stage3_line9_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage3_line9_2);
				if (Vector3::Distance(this->get_position(), stage3_line9_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage3_line9_3);
				if (Vector3::Distance(this->get_position(), stage3_line9_3) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(3);
				}
			}
			else if (m_path_checkPoint == 3) {
				set_aggro_direction(stage3_line9_4);
				if (Vector3::Distance(this->get_position(), stage3_line9_4) <= PATH_CHECKPOINT_RANGE) {
					std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 10) { // stage 3 path 10
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage3_line10_1);
				if (Vector3::Distance(this->get_position(), stage3_line10_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage3_line10_2);
				if (Vector3::Distance(this->get_position(), stage3_line10_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage3_line10_3);
				if (Vector3::Distance(this->get_position(), stage3_line10_3) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(3);
				}
			}
			else if (m_path_checkPoint == 3) {
				set_aggro_direction(stage3_line10_4);
				if (Vector3::Distance(this->get_position(), stage3_line10_4) <= PATH_CHECKPOINT_RANGE) {
					std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 11) { // stage 3 path 11
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage3_line11_1);
				if (Vector3::Distance(this->get_position(), stage3_line11_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage3_line11_2);
				if (Vector3::Distance(this->get_position(), stage3_line11_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage3_line11_3);
				if (Vector3::Distance(this->get_position(), stage3_line11_3) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(3);
				}
			}
			else if (m_path_checkPoint == 3) {
				set_aggro_direction(stage3_line11_4);
				if (Vector3::Distance(this->get_position(), stage3_line11_4) <= PATH_CHECKPOINT_RANGE) {
					std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
		else if (m_pathLine == 12) { // stage 3 path 12
			if (m_path_checkPoint == 0) {
				set_aggro_direction(stage3_line12_1);
				if (Vector3::Distance(this->get_position(), stage3_line12_1) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(1);
				}
			}
			else if (m_path_checkPoint == 1) {
				set_aggro_direction(stage3_line12_2);
				if (Vector3::Distance(this->get_position(), stage3_line12_2) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(2);
				}
			}
			else if (m_path_checkPoint == 2) {
				set_aggro_direction(stage3_line12_3);
				if (Vector3::Distance(this->get_position(), stage3_line12_3) <= PATH_CHECKPOINT_RANGE) {
					this->set_checkPoint(3);
				}
			}
			else if (m_path_checkPoint == 3) {
				set_aggro_direction(stage3_line12_4);
				if (Vector3::Distance(this->get_position(), stage3_line12_4) <= PATH_CHECKPOINT_RANGE) {
					std::cout << "Æ÷Å» µµÂø" << endl;
					this->m_arrive_portal = true;
				}
			}
			//this->move_forward(5.f);
			this->set_animation_state(M_ANIM_RUN);
		}
	}
}
