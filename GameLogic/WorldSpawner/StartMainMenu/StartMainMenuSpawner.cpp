#include"StartMainMenuSpawner.h"
#include"Widget/StartMainWidget/StartMainWidget.h"
#include"EngineFrameWork/World/World.h"
#include"Character/Player.h"

StartMainMenuSpawner::StartMainMenuSpawner()
{
}

StartMainMenuSpawner::~StartMainMenuSpawner()
{
}

void StartMainMenuSpawner::Spawn(World * world)
{
	world->SpawnObject<StartMainWidget>();
}
