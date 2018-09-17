#pragma once
#include"../../../EngineStatics/Engine.h"
#include"../../Box2D/Box2D.h"

class JG2DBody;
class JGCollisionDebugDraw;
class ENGINE_EXPORT JGPhysicsWorld
{
private:
	std::unique_ptr<b2World> m_World;
public:
	JGPhysicsWorld();
	~JGPhysicsWorld();
	JG2DBody* Create2DBody();
	void Step(const float DeltaTime, int32 velocityIterations, int32 positionIterations);
};