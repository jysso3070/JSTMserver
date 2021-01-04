#pragma once
#include "monster_path.h"
#include <concurrent_unordered_map.h>
#include "monster.h"

class MonsterSpawn
{
public:
	MonsterSpawn(){}
	~MonsterSpawn(){}

	void Stage1_SpawnMonster(Concurrency::concurrent_unordered_map<short, Monster*> &m_map_monsterPool, int room_number, int wave);
	void Stage2_SpawnMonster(Concurrency::concurrent_unordered_map<short, Monster*>& m_map_monsterPool, int room_number, int wave);
	void Stage3_SpawnMonster(Concurrency::concurrent_unordered_map<short, Monster*>& m_map_monsterPool, int room_number, int wave);

};

