#pragma once
#include"EngineFrameWork/Object/ObjectSpawner.h"



class DemoWorldSpawner : public ObjectSpawner
{
private:



public:
	DemoWorldSpawner();
	virtual ~DemoWorldSpawner();

	virtual void Spawn(World* world) override;
};