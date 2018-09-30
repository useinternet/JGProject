#include"Box2DCollisionComponent.h"
#include"../World/World.h"
#include"../../PhysicsSystem/JGBox2D/JGShape/JGPolygonShape.h"
#include"../../PhysicsSystem/JGBox2D/JGDynamics/JGPhysicsWorld.h"
#include"../../PhysicsSystem/JGBox2D/JGDynamics/JGFixture.h"


using namespace std;

Box2DCollisionComponent::Box2DCollisionComponent()
{
	m_Shape = make_unique<JGPolygonShape>();
	SetBodyType(E2DBodyType::Dynamic);
	m_CenterPos.Set(0.0f, 0.0f);
}

Box2DCollisionComponent::~Box2DCollisionComponent()
{

}
void Box2DCollisionComponent::BeginComponent(World* world)
{
	CollisionComponent::BeginComponent(world);
	SetBody(world->GetPyWorld()->Create2DBody(GetBodyType(),
		GetComponentLocationAddress(), GetComponentAngleAddress()));
	FixAngle();
	m_Shape->SetAsBox(m_HalfWidth, m_HalfHeight,m_CenterPos);
	GetBody()->CreateFixture(this, m_Shape.get(), GetDensity(), GetFriction(), GetRestitution());
	GetBody()->SetBodyFilter(GetFilter());
}
void Box2DCollisionComponent::SetAsBox(const float hw, const float hh, JGVector2D center)
{
	m_HalfWidth  = hw;
	m_HalfHeight = hh;
	m_CenterPos  = center;
	if (!m_bInit || !GetBody())
	{
		m_bInit = true;
		return;
	}
	m_Shape->SetAsBox(m_HalfWidth, m_HalfHeight,m_CenterPos);
	if (GetBody()->IsExistFixture())
	{
		GetBody()->DestroyFixture(0);
		GetBody()->CreateFixture(this, m_Shape.get(), GetDensity(), GetFriction(), GetRestitution());
		GetBody()->SetBodyFilter(GetFilter());
	}
}
