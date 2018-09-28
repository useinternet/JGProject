#pragma once
#include"../EngineFrameWork/Object/ExistObject.h"


class ENGINE_EXPORT TestGround : public ExistObject
{
	class Box2DCollisionComponent* Ground1;
	class Box2DCollisionComponent* Ground2;
	class Box2DCollisionComponent* Ground3;
	class Box2DCollisionComponent* Ground4;
	class Box2DCollisionComponent* Ground5;
public:
	TestGround();
	virtual~TestGround();
};