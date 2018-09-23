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
	SetBodyType(E2DBodyType::Dynamic);
}

Box2DCollisionComponent::~Box2DCollisionComponent()
{

}

void Box2DCollisionComponent::BeginComponent(World* world)
{
	CollisionComponent::BeginComponent(world);
	SetBody(world->GetPyWorld()->Create2DBody(GetBodyType(),
		GetComponentLocationAddress(), GetComponentAngleAddress()));
	m_Shape->SetAsBox(m_HalfWidth, m_HalfHeight);
	GetBody()->CreateFixture(this, m_Shape.get(), GetDensity(), GetFriction(), GetRestitution());
	GetBody()->SetBodyFilter(GetFilter());
}

void Box2DCollisionComponent::SetAsBox(const float hw, const float hh)
{
	m_HalfWidth = hw;
	m_HalfHeight = hh;
}