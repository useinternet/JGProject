#include"LoadingSpawner.h"
#include"EngineFrameWork/World/World.h"
#include"Widget/LoadingWidget/LoadingWidget.h"
LoadingSpawner::LoadingSpawner()
{
	RegisterObjectID(typeid(this));
}

LoadingSpawner::~LoadingSpawner()
{
}

void LoadingSpawner::Spawn(World* world)
{
	world->SpawnObject<LoadingWidget>();

}
