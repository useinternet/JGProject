#pragma once
#include"EngineFrameWork/Object/Object.h"

class SampleWorldSpawner : public Object
{
private:


public:
	SampleWorldSpawner();
	~SampleWorldSpawner();

	virtual void BeginObject(World* world) override;
};