#pragma once
#include"../../../EngineStatics/Engine.h"
#include"../../Box2D/Box2D.h"

class JG2DBody;
class JGVector2D;
class JGAngle2D;
enum class E2DBodyType;


class ENGINE_EXPORT JGPhysicsWorld
{
private:
	std::unique_ptr<b2World> m_World;
	std::vector<std::shared_ptr<JG2DBody>> m_vJGBody;
	static b2BodyDef m_BodyDef;

public:
	JGPhysicsWorld(b2Draw* DebugDraw,JGVector2D& Gravity);
	~JGPhysicsWorld();
	JG2DBody* Create2DBody(const E2DBodyType type,JGVector2D* LinkLocation, JGAngle2D* LinkAngle);
	void DestoryBody(JG2DBody* body);
	void Step(const float DeltaTime, int32 velocityIterations, int32 positionIterations);
	void DebugRender();
};