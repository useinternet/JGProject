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
	m_Shape->SetAsBox(m_HalfWidth, m_HalfHeight);
	GetBody()->CreateFixture(this, m_Shape.get(), GetDensity(), GetFriction(), GetRestitution());
	GetBody()->SetBodyFilter(GetFilter());
}

void Box2DCollisionComponent::SetAsBox(const float hw, const float hh, EReSizePivot Pivot)
{
	float prevHeight = m_HalfHeight;
	m_HalfWidth = hw;
	m_HalfHeight = hh;
	if (!m_bInit)
	{
		m_bInit = true;
		return;
	}
	m_Shape->SetAsBox(m_HalfWidth, m_HalfHeight);
	if (GetBody()->IsExistFixture())
	{
		GetBody()->DestroyFixture(0);
		GetBody()->CreateFixture(this, m_Shape.get(), GetDensity(), GetFriction(), GetRestitution());
		GetBody()->SetBodyFilter(GetFilter());

		JGVector2D vec;
		JGVector2D Location;
		JGAngle2D  Angle;
		switch (Pivot)
		{
		case EReSizePivot::Down:
			vec.Set(0.0f, prevHeight - m_HalfHeight);
			AddComponentLocation(vec);
			Location.Set(GetComponentWorldLocation());
			Angle.Set(GetComponentWorldAngle());
			GetBody()->SetTransform(Location, Angle);
			break;
		case EReSizePivot::Middle:
			break;
		case EReSizePivot::Up:
			vec.Set(0.0f, m_HalfHeight - prevHeight);
			AddComponentLocation(vec);
			Location.Set(GetComponentWorldLocation());
			Angle.Set(GetComponentWorldAngle());
			GetBody()->SetTransform(Location, Angle);
			break;
		}
	}
}