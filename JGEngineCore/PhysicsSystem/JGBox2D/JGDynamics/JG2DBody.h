#pragma once
#include"../../../EngineStatics/Engine.h"
#include"../../Box2D/Box2D.h"
class JGPhysicsWorld;
class JGVector2D;
class JGAngle2D;
class JGFixture;
class JGShape;
enum class ENGINE_EXPORT E2DBodyType
{
	Dynamic,
	Static,
	Kinematic
};
enum class ENGINE_EXPORT E2DBodyState
{
	Active,
	Hidden,
	Destory
};
class ENGINE_EXPORT JG2DBody
{
private:
	friend JGPhysicsWorld;
private:
	static b2FixtureDef m_FixtureDef;

	b2Body* m_Body = nullptr;
	JGVector2D* m_LinkLocation  = nullptr;
	JGAngle2D*  m_LinkAngle     = nullptr;
	E2DBodyType m_BodyType;
	E2DBodyState m_BodyState;
	std::vector<std::shared_ptr<JGFixture>> m_vFixtures;
public:
	JG2DBody();
	~JG2DBody();
	JGFixture* CreateFixture(JGShape* shape,const float friction = 0.2f,const float restitution = 0.0f,
	const float density = 0.0f, const bool isSensor = false);
	E2DBodyState GetBodyState();
private:
	void Tick();
	void CreateBody(b2Body* body,JGVector2D* LinkLocation, JGAngle2D* LinkAngle);
	b2Body* DestroyBody();
};