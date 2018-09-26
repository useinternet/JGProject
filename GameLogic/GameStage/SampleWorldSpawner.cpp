#include"SampleWorldSpawner.h"
#include"EngineFrameWork/World/World.h"
#include"Test/TestGround.h"
#include"Character/Player.h"
SampleWorldSpawner::SampleWorldSpawner()
{
}

SampleWorldSpawner::~SampleWorldSpawner()
{
}

void SampleWorldSpawner::BeginObject(World * world)
{
	Object::BeginObject(world);
	world->SpawnObject<TestGround>();
	world->SpawnObject<Player>();
}
