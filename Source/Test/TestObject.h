#pragma once
#include "Core.h"




class GAME_API TestObject : public GameObject
{
public:
	virtual void Begin() override;
	virtual void Tick() override;
	virtual void End() override;
};

JGCLASS(TestObject)