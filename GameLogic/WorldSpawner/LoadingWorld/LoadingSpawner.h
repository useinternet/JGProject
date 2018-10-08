#pragma once
#include"EngineFrameWork/Object/ObjectSpawner.h"


class LoadingSpawner : public ObjectSpawner
{
private:


public:
	LoadingSpawner();
	virtual ~LoadingSpawner();



	virtual void Spawn(World* world) override;
};