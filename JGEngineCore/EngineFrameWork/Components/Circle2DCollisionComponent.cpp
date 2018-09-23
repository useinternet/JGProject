#include"Circle2DCollisionComponent.h"
#include"../../PhysicsSystem/JGBox2D/JGShape/JGCircle.h"
#include"../World/World.h"
#include"../../PhysicsSystem/JGBox2D/JGDynamics/JGPhysicsWorld.h"
using namespace std;
Circle2DCollisionComponent::Circle2DCollisionComponent()
{
	RegisterComponentID(typeid(this));
	m_Circle = make_unique<JGCircle>();
}

Circle2DCollisionComponent::~Circle2DCollisionComponent()
{
}

void Circle2DCollisionComponent::BeginComponent(World* world)
{
	CollisionComponent::BeginComponent(world);
	SetBody(world->GetPyWorld()->Create2DBody(GetBodyType(),
		GetComponentLocationAddress(), GetComponentAngleAddress()));
	m_Circle->SetCircle(m_Radius);
	GetBody()->CreateFixture(this, m_Circle.get(), GetDensity(), GetFriction(), GetRestitution());
	GetBody()->SetBodyFilter(GetFilter());
}
void Circle2DCollisionComponent::SetRadius(const float Radius)
{
	m_Radius = Radius;
}
