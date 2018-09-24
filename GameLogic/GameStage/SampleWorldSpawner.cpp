#include"SampleWorldSpawner.h"
#include"EngineFrameWork/World/World.h"
#include"Test/TestGround.h"
SampleWorldSpawner::SampleWorldSpawner()
{
}

SampleWorldSpawner::~SampleWorldSpawner()
{
}

void SampleWorldSpawner::BeginObject(World * world)
{
	world->SpawnObject<TestGround>();
}
