#pragma once
#include"Object.h"


class ENGINE_EXPORT ObjectSpawner : public Object
{
private:

public:
	ObjectSpawner();
	virtual ~ObjectSpawner();
	virtual void Spawn(World* world);
};