#pragma once
#include"EngineFrameWork/Object/ObjectSpawner.h"


class StartMainMenuSpawner : public ObjectSpawner
{
private:

public:
	StartMainMenuSpawner();
	virtual ~StartMainMenuSpawner();
	virtual void Spawn(World* world) override;
};