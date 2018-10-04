#include "Side_Scroll_Unit.h"
#include"../Components/Box2DCollisionComponent.h"
#include"../../EngineStatics/JGLog.h"
Side_Scroll_Unit::Side_Scroll_Unit()
{
	RegisterObjectID(typeid(this));
	SetFallingCheck(EFallingCheck::None);
	SetDirectionCheck(EDirectionCheck::Right);
}

Side_Scroll_Unit::~Side_Scroll_Unit()
{
}

void Side_Scroll_Unit::DefineDirection()
{
	JGVector2D vel = GetCollision()->GetBody()->GetLinearVelocity();
	int velnX = (int)vel.X();
	int velnY = (int)vel.Y();


	if (velnY > 0)
	{
		SetFallingCheck(EFallingCheck::Down);
	}
	else if (velnY < 0)
	{
		SetFallingCheck(EFallingCheck::Up);
	}
	else
	{
		SetFallingCheck(EFallingCheck::None);
	}
	if (velnX > 0)
	{
		SetDirectionCheck(EDirectionCheck::Right);
		m_bStill = false;
	}
	else if (velnX < 0)
	{
		SetDirectionCheck(EDirectionCheck::Left);
		m_bStill = false;
	}
	else
	{
		m_bStill = true;
	}
}

void Side_Scroll_Unit::DefineMove()
{
	JGVector2D vel = GetCollision()->GetBody()->GetLinearVelocity();
	float velChangeX = GetVelocity().X() - vel.X();
	JGVector2D vecImpulse(GetCollision()->GetBody()->GetMass() * velChangeX, 0.0f);
	GetCollision()->GetBody()->ApplyLinearImpulse(vecImpulse);
}

bool Side_Scroll_Unit::IsStill()
{
	return m_bStill;
}

bool Side_Scroll_Unit::IsRight()
{
	if (GetDirectionCheck() == EDirectionCheck::Right)
	{
		return true;
	}
	return false;
}
bool Side_Scroll_Unit::IsLeft()
{
	if (GetDirectionCheck() == EDirectionCheck::Left)
	{
		return true;
	}
	return false;
}
bool Side_Scroll_Unit::IsFalling()
{
	if (GetFallingCheck() == EFallingCheck::None)
	{
		return false;
	}
	return true;
}
bool Side_Scroll_Unit::IsFallingDown()
{
	if (GetFallingCheck() == EFallingCheck::Down)
	{
		return true;
	}
	return false;
}
bool Side_Scroll_Unit::IsFallingUp()
{
	if (GetFallingCheck() == EFallingCheck::Up)
	{
		return true;
	}
	return false;
}

void Side_Scroll_Unit::NotifyCompulsoryRight()
{
	SetDirectionCheck(EDirectionCheck::Right);
	m_bCompulsoryDirection = true;
}

void Side_Scroll_Unit::NotifyCompulsoryLeft()
{
	SetDirectionCheck(EDirectionCheck::Left);
	m_bCompulsoryDirection = true;
}

bool Side_Scroll_Unit::IsCompulsoryDirection()
{
	return m_bCompulsoryDirection;
}

void Side_Scroll_Unit::NotifySolveCompulsoryDirection()
{
	m_bCompulsoryDirection = false;
}

void Side_Scroll_Unit::PushUp(const float ContantForce)
{
	JGVector2D vel(0.0f, 0.0f);
	vel.Set(0.0, -GetCollision()->GetBody()->GetMass() * ContantForce);
	GetCollision()->GetBody()->ApplyLinearImpulse(vel);
}
