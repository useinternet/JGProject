#pragma once
#include"EngineFrameWork/Object/ObjectSpawner.h"


class BattleMapSpawner : public ObjectSpawner
{
private:


public:
	BattleMapSpawner();
	virtual ~BattleMapSpawner();
	virtual void Spawn(World* world) override;

};