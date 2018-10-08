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
	if (m_PrevDirection != m_CurrentDirection ||
		m_PrevFallingCheck != m_FallingCheck)
	{
		m_PrevFallingCheck = m_FallingCheck;
		m_PrevDirection = m_CurrentDirection;
		m_bWorking = true;
	}
	else
	{
		m_bWorking = false;
	}

	DefineMove();
	DefineDirection();
}
void Unit::DefineDirection() {}
void Unit::DefineMove() {}
Box2DCollisionComponent* Unit::GetCollision()
{
	return m_Collision;
}


bool Unit::IsWorking()
{
	return m_bWorking;
}


FallingCheck Unit::GetFallingCheck()
{
	return m_FallingCheck;
}
DirectionCheck Unit::GetDirectionCheck()
{
	return m_CurrentDirection;
}
void Unit::SetFallingCheck(FallingCheck check)
{
	m_FallingCheck = check;
}
void Unit::SetDirectionCheck(DirectionCheck check)
{
	m_CurrentDirection = check;
}
void Unit::SetVelocity(const JGVector2D& vel)
{
	m_Velocity.Set(vel);
}

void Unit::SetVelocityX(const float velX)
{
	m_Velocity.SetX(velX);
}

void Unit::SetVelocityY(const float velY)
{
	m_Velocity.SetY(velY);
}

JGVector2D& Unit::GetVelocity()
{
	return m_Velocity;
}

JGVector2D Unit::GetObjectLocation()
{
	return m_Collision->GetComponentWorldLocation();
}




