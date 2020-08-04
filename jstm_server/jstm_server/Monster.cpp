#include "Monster.h"



Monster::Monster()
{
	m_buffType = TRAP_BUFF_NONE;
	m_checkPoint_1 = XMFLOAT3(0.f, 0.f, 0.f);
	m_checkPoint_2 = XMFLOAT3(0.f, 0.f, 0.f);
	m_checkPoint_3 = XMFLOAT3(0.f, 0.f, 0.f);
	m_checkPoint_4 = XMFLOAT3(0.f, 0.f, 0.f);
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
			m_checkPoint_3 = stage1_line1_3;
		}
		else if (m_pathLine == 4 || m_pathLine == 5 || m_pathLine == 6) {
			m_checkPoint_1 = XMFLOAT3(430.f + stage1_check1(dre), -50.f, 3030.f + stage1_check1(dre));
			m_checkPoint_2 = XMFLOAT3(90.f + stage1_check2(dre), -50.f, 2900.f + stage1_check2(dre));
			m_checkPoint_3 = stage1_line1_3;
		}
	}
	else if (m_stage_number == 2) {
	}

	else if (m_stage_number == 3) {

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
				std::cout << "Æ÷Å» µµÂø" << endl;
				this->m_arrive_portal = true;
			}
		}
		//this->move_forward(5.f);
		this->set_animation_state(M_ANIM_RUN);
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
