#include "MonsterSpawn.h"

void MonsterSpawn::Stage1_SpawnMonster(Concurrency::concurrent_unordered_map<short, Monster*>& m_map_monsterPool, int room_number, int wave )
{
	XMFLOAT3 line1 = stage1_line1_start;
	XMFLOAT3 line4 = stage1_line4_start;
	switch (wave)
	{
	case 1:		// wave1
	{
		for (int i = 0; i < 30; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 15) { // line 123
				if (i >= 14) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(1, 1, XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
					(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < 30) { // line 456
				m_map_monsterPool[room_number][i].gen_sequence(1, 4, XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
					(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
		}
		break;
	}
	case 2:		// wave2
	{
		for (int i = 0; i < 36; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 18) { // line 123
				if (i >= 15) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(1, 1, XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
					(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < 36) { // line 456
				if (i >= 33) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(1, 4, XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
					(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
		}
		break;
	}
	case 3:		// wave3
	{
		for (int i = 0; i < 36; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 18) { // line 123
				if (i >= 12) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(1, 1, XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
					(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < 36) { // line 456
				if (i >= 30) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(1, 4, XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
					(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
		}
		break;
	}
	case 4:
	{
		short waveMax = 42;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 21) { // line 123
				if (i >= 15 && i < 18) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 18) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(1, 1, XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
					(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < waveMax) { // line 456
				if (i >= 36 && i < 39) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 39) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(1, 4, XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
					(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
		}
		break;
	}
	case 5:
	{
		short waveMax = 42;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 21) { // line 123
				if (i >= 15 && i < 18) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 18) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(1, 1, XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
					(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < waveMax) { // line 456
				if (i >= 36 && i < 39) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 39) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(1, 4, XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
					(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
		}
		break;
	}
	case 6:
	{
		short waveMax = 48;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 24) { // line 123
				if (i >= 15 && i < 21) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 21) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(1, 1, XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
					(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < waveMax) { // line 456
				if (i >= 39 && i < 45) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 45) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(1, 4, XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
					(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
		}
		break;
	}
	case 7:
	{
		short waveMax = 48;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 24) { // line 123
				if (i >= 15 && i < 21) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 21) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(1, 1, XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
					(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < waveMax) { // line 456
				if (i >= 39 && i < 45) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 45) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(1, 4, XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
					(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
		}
		break;
	}
	case 8:
	{
		short waveMax = 54;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 27) { // line 123
				if (i >= 15 && i < 24) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 24) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(1, 1, XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
					(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < waveMax) { // line 456
				if (i >= 42 && i < 51) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 51) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(1, 4, XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
					(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
		}
		break;
	}
	case 9:
	{
		short waveMax = 54;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 27) { // line 123
				if (i >= 15 && i < 21) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 21) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(1, 1, XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
					(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < waveMax) { // line 456
				if (i >= 42 && i < 48) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 48) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(1, 4, XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
					(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
		}
		break;
	}
	case 10:
	{
		short waveMax = 54;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 27) { // line 123
				if (i >= 11 && i < 19) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 19) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(1, 1, XMFLOAT3(line1.x + (float)stage1_start123_x(dre), line1.y,
					(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < waveMax) { // line 456
				if (i >= 38 && i < 46) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 46) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(1, 4, XMFLOAT3(line4.x + (float)stage1_start123_x(dre), line4.y,
					(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
		}
		break;
	}
	default:
		break;
	}
}

void MonsterSpawn::Stage2_SpawnMonster(Concurrency::concurrent_unordered_map<short, Monster*>& m_map_monsterPool, int room_number, int wave)
{
	XMFLOAT3 line1 = stage2_line1_start;
	XMFLOAT3 line4 = stage2_line4_start;
	XMFLOAT3 line7 = stage2_line7_start;
	XMFLOAT3 line10 = stage2_line10_start;
	switch (wave) {
	case 1: {
		short waveMax = 28;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 7) { // line 123
				m_map_monsterPool[room_number][i].gen_sequence(2, 1, XMFLOAT3((line1.x + (float)stage2_start1_x(dre)), line1.y,
					(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < 14) { // line 456
				m_map_monsterPool[room_number][i].gen_sequence(2, 4, XMFLOAT3((line4.x + (float)stage2_start1_x(dre)), line4.y,
					(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < 21) { // line 789
				m_map_monsterPool[room_number][i].gen_sequence(2, 7, XMFLOAT3((line7.x + (float)stage2_start1_x(dre)), line7.y,
					(line7.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < waveMax) { // line 10 11 12
				m_map_monsterPool[room_number][i].gen_sequence(2, 10, XMFLOAT3((line10.x + (float)stage2_start1_x(dre)), line10.y,
					(line10.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
		}
		break;
	}
	case 2: {
		short waveMax = 28;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 7) { // line 123
				m_map_monsterPool[room_number][i].gen_sequence(2, 1, XMFLOAT3((line1.x + (float)stage2_start1_x(dre)), line1.y,
					(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < 14) { // line 456
				m_map_monsterPool[room_number][i].gen_sequence(2, 4, XMFLOAT3((line4.x + (float)stage2_start1_x(dre)), line4.y,
					(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < 21) { // line 789
				m_map_monsterPool[room_number][i].gen_sequence(2, 7, XMFLOAT3((line7.x + (float)stage2_start1_x(dre)), line7.y,
					(line7.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < waveMax) { // line 10 11 12
				m_map_monsterPool[room_number][i].gen_sequence(2, 10, XMFLOAT3((line10.x + (float)stage2_start1_x(dre)), line10.y,
					(line10.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
		}
		break;
	}
	case 3: {
		short waveMax = 36;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 9) { // line 123
				if (i >= 7) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 1, XMFLOAT3((line1.x + (float)stage2_start1_x(dre)), line1.y,
					(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < 18) { // line 456
				if (i >= 16) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 4, XMFLOAT3((line4.x + (float)stage2_start1_x(dre)), line4.y,
					(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < 27) { // line 789
				if (i >= 25) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 7, XMFLOAT3((line7.x + (float)stage2_start1_x(dre)), line7.y,
					(line7.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < waveMax) { // line 10 11 12
				if (i >= 34) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 10, XMFLOAT3((line10.x + (float)stage2_start1_x(dre)), line10.y,
					(line10.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
		}
		break;
	}
	case 4: {
		short waveMax = 36;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 9) { // line 123
				if (i >= 6) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 1, XMFLOAT3((line1.x + (float)stage2_start1_x(dre)), line1.y,
					(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < 18) { // line 456
				if (i >= 15) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 4, XMFLOAT3((line4.x + (float)stage2_start1_x(dre)), line4.y,
					(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < 27) { // line 789
				if (i >= 24) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 7, XMFLOAT3((line7.x + (float)stage2_start1_x(dre)), line7.y,
					(line7.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < waveMax) { // line 10 11 12
				if (i >= 33) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 10, XMFLOAT3((line10.x + (float)stage2_start1_x(dre)), line10.y,
					(line10.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
		}
		break;
	}
	case 5: {
		short waveMax = 44;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 11) { // line 123
				if (i >= 8) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 1, XMFLOAT3((line1.x + (float)stage2_start1_x(dre)), line1.y,
					(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < 22) { // line 456
				if (i >= 19) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 4, XMFLOAT3((line4.x + (float)stage2_start1_x(dre)), line4.y,
					(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < 33) { // line 789
				if (i >= 30) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 7, XMFLOAT3((line7.x + (float)stage2_start1_x(dre)), line7.y,
					(line7.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < waveMax) { // line 10 11 12
				if (i >= 41) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 10, XMFLOAT3((line10.x + (float)stage2_start1_x(dre)), line10.y,
					(line10.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
		}
		break;
	}
	case 6: {
		short waveMax = 52;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 13) { // line 123
				if (i >= 8 && i < 11) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 11) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 1, XMFLOAT3((line1.x + (float)stage2_start1_x(dre)), line1.y,
					(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < 26) { // line 456
				if (i >= 21 && i < 24) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 24) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 4, XMFLOAT3((line4.x + (float)stage2_start1_x(dre)), line4.y,
					(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < 39) { // line 789
				if (i >= 34 && i < 37) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 37) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 7, XMFLOAT3((line7.x + (float)stage2_start1_x(dre)), line7.y,
					(line7.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < waveMax) { // line 10 11 12
				if (i >= 47 && i < 50) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 50) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 10, XMFLOAT3((line10.x + (float)stage2_start1_x(dre)), line10.y,
					(line10.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
		}
		break;
	}
	case 7: {
		short waveMax = 60;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 15) { // line 123
				if (i >= 8 && i < 13) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 13) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 1, XMFLOAT3((line1.x + (float)stage2_start1_x(dre)), line1.y,
					(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < 30) { // line 456
				if (i >= 23 && i < 28) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 28) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 4, XMFLOAT3((line4.x + (float)stage2_start1_x(dre)), line4.y,
					(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < 45) { // line 789
				if (i >= 38 && i < 43) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 43) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 7, XMFLOAT3((line7.x + (float)stage2_start1_x(dre)), line7.y,
					(line7.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < waveMax) { // line 10 11 12
				if (i >= 53 && i < 58) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 58) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 10, XMFLOAT3((line10.x + (float)stage2_start1_x(dre)), line10.y,
					(line10.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
		}
		break;
	}
	case 8: {
		short waveMax = 64;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 16) { // line 123
				if (i >= 8 && i < 13) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 13) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 1, XMFLOAT3((line1.x + (float)stage2_start1_x(dre)), line1.y,
					(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < 32) { // line 456
				if (i >= 24 && i < 29) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 29) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 4, XMFLOAT3((line4.x + (float)stage2_start1_x(dre)), line4.y,
					(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < 48) { // line 789
				if (i >= 40 && i < 45) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 45) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 7, XMFLOAT3((line7.x + (float)stage2_start1_x(dre)), line7.y,
					(line7.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < waveMax) { // line 10 11 12
				if (i >= 56 && i < 61) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 61) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 10, XMFLOAT3((line10.x + (float)stage2_start1_x(dre)), line10.y,
					(line10.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
		}
		break;
	}
	case 9: {
		short waveMax = 68;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 17) { // line 123
				if (i >= 8 && i < 13) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 13) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 1, XMFLOAT3((line1.x + (float)stage2_start1_x(dre)), line1.y,
					(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < 34) { // line 456
				if (i >= 25 && i < 30) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 30) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 4, XMFLOAT3((line4.x + (float)stage2_start1_x(dre)), line4.y,
					(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < 51) { // line 789
				if (i >= 42 && i < 47) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 47) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 7, XMFLOAT3((line7.x + (float)stage2_start1_x(dre)), line7.y,
					(line7.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < waveMax) { // line 10 11 12
				if (i >= 59 && i < 64) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 64) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 10, XMFLOAT3((line10.x + (float)stage2_start1_x(dre)), line10.y,
					(line10.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
		}
		break;
	}
	case 10: {
		short waveMax = 68;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 17) { // line 123
				if (i >= 7 && i < 13) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 13) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 1, XMFLOAT3((line1.x + (float)stage2_start1_x(dre)), line1.y,
					(line1.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < 34) { // line 456
				if (i >= 24 && i < 30) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 30) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 4, XMFLOAT3((line4.x + (float)stage2_start1_x(dre)), line4.y,
					(line4.z + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < 51) { // line 789
				if (i >= 41 && i < 47) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 47) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 7, XMFLOAT3((line7.x + (float)stage2_start1_x(dre)), line7.y,
					(line7.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
			else if (i < waveMax) { // line 10 11 12
				if (i >= 58 && i < 64) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 64) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(2, 10, XMFLOAT3((line10.x + (float)stage2_start1_x(dre)), line10.y,
					(line10.z - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE)));
			}
		}
		break;
	}
	default:
		break;
	}
}

void MonsterSpawn::Stage3_SpawnMonster(Concurrency::concurrent_unordered_map<short, Monster*>& m_map_monsterPool, int room_number, int wave)
{
	XMFLOAT3 line1 = stage3_line1_start;
	XMFLOAT3 line4 = stage3_line4_start;
	XMFLOAT3 line9 = stage3_line9_start;
	XMFLOAT3 line12 = stage3_line12_start;
	switch (wave) {
	case 1:
	{
		short waveMax = 28;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 7) { // line 123
				m_map_monsterPool[room_number][i].gen_sequence(3, 1, XMFLOAT3((line1.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line1.y,
					(line1.z + (float)stage3_start1_z(dre))));
			}
			else if (i < 14) { // line 456
				m_map_monsterPool[room_number][i].gen_sequence(3, 4, XMFLOAT3((line4.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line4.y,
					(line4.z + (float)stage3_start1_z(dre))));
			}
			else if (i < 21) { // line 789
				m_map_monsterPool[room_number][i].gen_sequence(3, 7, XMFLOAT3((line9.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line9.y,
					(line9.z + (float)stage3_start1_z(dre))));
			}
			else if (i < waveMax) { // line 10 11 12
				m_map_monsterPool[room_number][i].gen_sequence(3, 10, XMFLOAT3((line12.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line12.y,
					(line12.z + (float)stage3_start1_z(dre))));
			}
		}
		break;
	}
	case 2:
	{
		short waveMax = 28;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 7) { // line 123
				m_map_monsterPool[room_number][i].gen_sequence(3, 1, XMFLOAT3((line1.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line1.y,
					(line1.z + (float)stage3_start1_z(dre))));
			}
			else if (i < 14) { // line 456
				m_map_monsterPool[room_number][i].gen_sequence(3, 4, XMFLOAT3((line4.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line4.y,
					(line4.z + (float)stage3_start1_z(dre))));
			}
			else if (i < 21) { // line 789
				m_map_monsterPool[room_number][i].gen_sequence(3, 7, XMFLOAT3((line9.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line9.y,
					(line9.z + (float)stage3_start1_z(dre))));
			}
			else if (i < waveMax) { // line 10 11 12
				m_map_monsterPool[room_number][i].gen_sequence(3, 10, XMFLOAT3((line12.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line12.y,
					(line12.z + (float)stage3_start1_z(dre))));
			}
		}
		break;
	}
	case 3:
	{
		short waveMax = 36;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 9) { // line 123
				if (i >= 7) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 1, XMFLOAT3((line1.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line1.y,
					(line1.z + (float)stage3_start1_z(dre))));
			}
			else if (i < 18) { // line 456
				if (i >= 16) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 4, XMFLOAT3((line4.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line4.y,
					(line4.z + (float)stage3_start1_z(dre))));
			}
			else if (i < 27) { // line 789
				if (i >= 25) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 7, XMFLOAT3((line9.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line9.y,
					(line9.z + (float)stage3_start1_z(dre))));
			}
			else if (i < waveMax) { // line 10 11 12
				if (i >= 34) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 10, XMFLOAT3((line12.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line12.y,
					(line12.z + (float)stage3_start1_z(dre))));
			}
		}
		break;
	}
	case 4:
	{
		short waveMax = 36;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 9) { // line 123
				if (i >= 6) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 1, XMFLOAT3((line1.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line1.y,
					(line1.z + (float)stage3_start1_z(dre))));
			}
			else if (i < 18) { // line 456
				if (i >= 15) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 4, XMFLOAT3((line4.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line4.y,
					(line4.z + (float)stage3_start1_z(dre))));
			}
			else if (i < 27) { // line 789
				if (i >= 24) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 7, XMFLOAT3((line9.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line9.y,
					(line9.z + (float)stage3_start1_z(dre))));
			}
			else if (i < waveMax) { // line 10 11 12
				if (i >= 33) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 10, XMFLOAT3((line12.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line12.y,
					(line12.z + (float)stage3_start1_z(dre))));
			}
		}
		break;
	}
	case 5:
	{
		short waveMax = 44;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 11) { // line 123
				if (i >= 8) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 1, XMFLOAT3((line1.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line1.y,
					(line1.z + (float)stage3_start1_z(dre))));
			}
			else if (i < 22) { // line 456
				if (i >= 19) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 4, XMFLOAT3((line4.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line4.y,
					(line4.z + (float)stage3_start1_z(dre))));
			}
			else if (i < 33) { // line 789
				if (i >= 30) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 7, XMFLOAT3((line9.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line9.y,
					(line9.z + (float)stage3_start1_z(dre))));
			}
			else if (i < waveMax) { // line 10 11 12
				if (i >= 41) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 10, XMFLOAT3((line12.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line12.y,
					(line12.z + (float)stage3_start1_z(dre))));
			}
		}
		break;
	}
	case 6:
	{
		short waveMax = 52;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 13) { // line 123
				if (i >= 8 && i < 11) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 11) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 1, XMFLOAT3((line1.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line1.y,
					(line1.z + (float)stage3_start1_z(dre))));
			}
			else if (i < 26) { // line 456
				if (i >= 19 && i < 24) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 24) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 4, XMFLOAT3((line4.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line4.y,
					(line4.z + (float)stage3_start1_z(dre))));
			}
			else if (i < 39) { // line 789
				if (i >= 34 && i < 37) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 37) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 7, XMFLOAT3((line9.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line9.y,
					(line9.z + (float)stage3_start1_z(dre))));
			}
			else if (i < waveMax) { // line 10 11 12
				if (i >= 47 && i < 50) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 50) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 10, XMFLOAT3((line12.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line12.y,
					(line12.z + (float)stage3_start1_z(dre))));
			}
		}
		break;
	}
	case 7:
	{
		short waveMax = 60;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 15) { // line 123
				if (i >= 8 && i < 13) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 13) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 1, XMFLOAT3((line1.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line1.y,
					(line1.z + (float)stage3_start1_z(dre))));
			}
			else if (i < 30) { // line 456
				if (i >= 23 && i < 28) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 28) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 4, XMFLOAT3((line4.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line4.y,
					(line4.z + (float)stage3_start1_z(dre))));
			}
			else if (i < 45) { // line 789
				if (i >= 38 && i < 43) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 43) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 7, XMFLOAT3((line9.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line9.y,
					(line9.z + (float)stage3_start1_z(dre))));
			}
			else if (i < waveMax) { // line 10 11 12
				if (i >= 53 && i < 58) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 58) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 10, XMFLOAT3((line12.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line12.y,
					(line12.z + (float)stage3_start1_z(dre))));
			}
		}
		break;
	}
	case 8:
	{
		short waveMax = 64;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 16) { // line 123
				if (i >= 8 && i < 13) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 13) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 1, XMFLOAT3((line1.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line1.y,
					(line1.z + (float)stage3_start1_z(dre))));
			}
			else if (i < 32) { // line 456
				if (i >= 24 && i < 29) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 29) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 4, XMFLOAT3((line4.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line4.y,
					(line4.z + (float)stage3_start1_z(dre))));
			}
			else if (i < 48) { // line 789
				if (i >= 40 && i < 45) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 45) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 7, XMFLOAT3((line9.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line9.y,
					(line9.z + (float)stage3_start1_z(dre))));
			}
			else if (i < waveMax) { // line 10 11 12
				if (i >= 56 && i < 61) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 61) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 10, XMFLOAT3((line12.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line12.y,
					(line12.z + (float)stage3_start1_z(dre))));
			}
		}
		break;
	}
	case 9:
	{
		short waveMax = 68;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 17) { // line 123
				if (i >= 8 && i < 13) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 13) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 1, XMFLOAT3((line1.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line1.y,
					(line1.z + (float)stage3_start1_z(dre))));
			}
			else if (i < 34) { // line 456
				if (i >= 25 && i < 30) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 30) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 4, XMFLOAT3((line4.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line4.y,
					(line4.z + (float)stage3_start1_z(dre))));
			}
			else if (i < 51) { // line 789
				if (i >= 42 && i < 47) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 47) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 7, XMFLOAT3((line9.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line9.y,
					(line9.z + (float)stage3_start1_z(dre))));
			}
			else if (i < waveMax) { // line 10 11 12
				if (i >= 59 && i < 64) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 64) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 10, XMFLOAT3((line12.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line12.y,
					(line12.z + (float)stage3_start1_z(dre))));
			}
		}
		break;
	}
	case 10:
	{
		short waveMax = 68;
		for (int i = 0; i < waveMax; ++i) {
			m_map_monsterPool[room_number][i].set_monster_type(TYPE_ORC);
			if (i < 17) { // line 123
				if (i >= 7 && i < 13) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 13) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 1, XMFLOAT3((line1.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line1.y,
					(line1.z + (float)stage3_start1_z(dre))));
			}
			else if (i < 34) { // line 456
				if (i >= 24 && i < 30) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 30) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 4, XMFLOAT3((line4.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line4.y,
					(line4.z + (float)stage3_start1_z(dre))));
			}
			else if (i < 51) { // line 789
				if (i >= 41 && i < 47) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 47) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 7, XMFLOAT3((line9.x + (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line9.y,
					(line9.z + (float)stage3_start1_z(dre))));
			}
			else if (i < waveMax) { // line 10 11 12
				if (i >= 58 && i < 64) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_STRONGORC);
				}
				else if (i >= 64) {
					m_map_monsterPool[room_number][i].set_monster_type(TYPE_RIDER);
				}
				m_map_monsterPool[room_number][i].gen_sequence(3, 10, XMFLOAT3((line12.x - (i % SPAWN_DIS) * MONSTER_GEN_DISTANCE), line12.y,
					(line12.z + (float)stage3_start1_z(dre))));
			}
		}
		break;
	}
	default:
		break;
	}
}
