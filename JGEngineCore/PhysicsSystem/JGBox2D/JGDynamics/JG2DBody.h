#pragma once
#include"../../../EngineStatics/Engine.h"
#include"../../Box2D/Box2D.h"
class CollisionComponent;
class JGPhysicsWorld;
class JGVector2D;
class JGAngle2D;
class JGFixture;
class JGShape;
class JG2DFilter;
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
	b2ContactListener c;
	friend JGPhysicsWorld;
private:
	static b2FixtureDef m_FixtureDef;
	b2Body* m_Body = nullptr;
	JGVector2D*  m_LinkLocation  = nullptr;
	JGAngle2D*   m_LinkAngle     = nullptr;
	E2DBodyType  m_BodyType;
	E2DBodyState m_BodyState;
	std::vector<std::shared_ptr<JGFixture>> m_vFixtures;
	bool m_IsPhysics = true;
public:
	JG2DBody();
	~JG2DBody();
	b2Body* origin();
	JGFixture* CreateFixture(CollisionComponent* Collision,JGShape* shape, const float density = 0.0f,
		const float friction = 0.2f,const float restitution = 0.0f, const bool isSensor = false);
	void  DestroyFixture(const size_t idx);
	bool  IsExistFixture();
	void  PhysicsOff();
	void  PhysicsOn(bool IsFixedRotation = false);
	void  SetBodyFilter(JG2DFilter& filter);
	void  SetLinearVelocity(JGVector2D& velocity);
	void  ApplyForce(JGVector2D& force,bool wake = true);
	void  ApplyLinearImpulse(JGVector2D& impulse, bool wake = true);
	void  ApplyLinearImpulseToCenter(JGVector2D& impulse, bool wake = true);
	float GetMass();
	JGVector2D   GetLinearVelocity();
	E2DBodyState GetBodyState();
	void SetTransform(JGVector2D& Location, JGAngle2D& Angle);
	JGFixture* GetFixture(const size_t idx);
	std::vector<std::shared_ptr<JGFixture>>* GetFixtureList();
private:
	void Tick();
	void CreateBody(b2Body* body,JGVector2D* LinkLocation, JGAngle2D* LinkAngle);
	b2Body* DestroyBody();
};