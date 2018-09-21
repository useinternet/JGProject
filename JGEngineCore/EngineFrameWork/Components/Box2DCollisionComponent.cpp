#include"Box2DCollisionComponent.h"
#include"../World/World.h"
#include"../../PhysicsSystem/JGBox2D/JGShape/JGPolygonShape.h"
#include"../../PhysicsSystem/JGBox2D/JGDynamics/JGPhysicsWorld.h"
#include"../../PhysicsSystem/JGBox2D/JGDynamics/JGFixture.h"


#include"../../EngineStatics/JGLog.h"
using namespace std;

Box2DCollisionComponent::Box2DCollisionComponent()
{
	m_Shape = make_unique<JGPolygonShape>();
	m_BodyType = E2DBodyType::Dynamic;
}

Box2DCollisionComponent::~Box2DCollisionComponent()
{

}

void Box2DCollisionComponent::BeginComponent(World* world)
{
	CollisionComponent::BeginComponent(world);
	m_Body = world->GetPyWorld()->Create2DBody(m_BodyType,
		GetComponentLocationAddress(), GetComponentAngleAddress());
	m_Shape->SetAsBox(m_HalfWidth, m_HalfHeight);
	m_Body->CreateFixture(this, m_Shape.get(), m_Density, 0.0f, m_Restitution);
}

void Box2DCollisionComponent::Tick(const float DeltaTime)
{
	CollisionComponent::Tick(DeltaTime);
}

void Box2DCollisionComponent::SetCollisionType(const E2DBodyType type)
{
	m_BodyType = type;
}

void Box2DCollisionComponent::SetAsBox(const float hw, const float hh)
{
	m_HalfWidth = hw;
	m_HalfHeight = hh;
}

void Box2DCollisionComponent::SetDensity(const float density)
{
	m_Density = density;
}

void Box2DCollisionComponent::SetFriction(const float friction)
{
	m_Friction = friction;
}

void Box2DCollisionComponent::SetRestitution(const float restitution)
{
	m_Restitution = restitution;
}

JG2DBody* Box2DCollisionComponent::GetBody()
{
	return m_Body;
}
