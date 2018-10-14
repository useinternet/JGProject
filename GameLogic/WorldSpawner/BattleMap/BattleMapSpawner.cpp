#include"BattleMapSpawner.h"
#include"EngineFrameWork/World/World.h"
#include"Map/BattleMap_00.h"
#include"Character/Player.h"
#include"EnemyUnit/EnemyUnitBase.h"
#include"Widget/GameWidget/PlayerStatusWidget.h"
BattleMapSpawner::BattleMapSpawner()
{
	RegisterObjectID(typeid(this));
}

BattleMapSpawner::~BattleMapSpawner()
{
}

void BattleMapSpawner::Spawn(World * world)
{
	ObjectSpawner::Spawn(world);
	
	world->SpawnObject<BattlMap_00>();
	world->SpawnObject<Player>();
	world->SpawnObject<EnemyUnitBase>();
	world->SpawnObject<PlayerStatusWidget>();
}
