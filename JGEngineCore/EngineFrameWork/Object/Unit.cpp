#include"Unit.h"
#include"../Components/Box2DCollisionComponent.h"
#include"../../EngineStatics/JMath/JGMath.h"
#include"../../EngineStatics/JGLog.h"

static const float FallingErrorRange = 0.05f;
Unit::Unit()
{
	RegisterObjectID(typeid(this));

	m_Collision = RegisterComponentInObject<Box2DCollisionComponent>(TT("UnitBoxCollision"));
	m_Collision->SetBodyType(E2DBodyType::Dynamic);
	m_FallingCheck = FallingCheck::None;
	m_CurrentDirection = DirectionCheck::Right;

}

Unit::~Unit()
{
}

void Unit::BeginObject(World* world)
{
	ExistObject::BeginObject(world);
	// 물리 끄고 키기..
	m_Collision->GetBody()->PhysicsOn();
	m_Collision->FixAngle();
}

void Unit::Tick(const float DeltaTime)
{
	ExistObject::Tick(DeltaTime);

	int VelY = (int)m_Collision->GetBody()->GetLinearVelocity().Y();
	int VelX = (int)m_Collision->GetBody()->GetLinearVelocity().X();


	if (VelY > 0)
	{
		m_FallingCheck = FallingCheck::Down;
	}
	else if (VelY < 0)
	{
		m_FallingCheck = FallingCheck::Up;
	}
	else
	{
		m_FallingCheck = FallingCheck::None;

	}
	if (VelX > 0)
	{
		m_CurrentDirection = DirectionCheck::Right;
		m_bStill = false;
	}
	else if (VelX < 0)
	{
		m_CurrentDirection = DirectionCheck::Left;
		m_bStill = false;
	}
	else
	{
		m_bStill = true;
	}
}

Box2DCollisionComponent* Unit::GetCollision()
{
	return m_Collision;
}


bool Unit::IsRight()
{
	if (m_CurrentDirection == DirectionCheck::Right)
	{
		return true;
	}
	return false;
}
bool Unit::IsLeft()
{
	if (m_CurrentDirection == DirectionCheck::Left)
	{
		return true;
	}
	return false;
}
bool Unit::IsStill()
{
	return m_bStill;
}
bool Unit::IsFalling()
{
	if (m_FallingCheck == FallingCheck::None)
	{
		return false;
	}
	return true;
}
bool Unit::IsFallingDown()
{
	if (m_FallingCheck == FallingCheck::Down)
	{
		return true;
	}
	return false;
}
bool Unit::IsFallingUp()
{
	if (m_FallingCheck == FallingCheck::Up)
	{
		return true;
	}
	return false;
}
