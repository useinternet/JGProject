#include"Unit.h"
#include"../Components/Box2DCollisionComponent.h"
#include"../DamageInformation/SingleDamage.h"
#include"../DamageInformation/SplashDamage.h"
#include"../../EngineStatics/JMath/JGMath.h"
#include"../../EngineStatics/JGLog.h"
using namespace std;

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
	m_Collision->SetBeginOverlapEvent([this](Object* obj) {
		this->BeginOverlap(obj);
	});
	m_Collision->SetEndOverlapEvent([this](Object* obj) {
		this->EndOverlap(obj);
	});
	m_Collision->SetOverlappingEvent([this](const std::vector<Object*>& ObjList) {
		this->Overlapping(ObjList);
	});
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
	if (m_bDamaged)
	{
		m_bDamaged = false;
		ReceiveDamage();
	}
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

void Unit::BeginOverlap(Object* obj)
{

}
void Unit::EndOverlap(Object* obj)
{

}
void Unit::Overlapping(const vector<Object*>& ObjList)
{
}

void Unit::SendDamage(DamageInformationBase* dmg)
{
	m_bDamaged = true;
	SingleDamage* singleDmg = nullptr;
	SplashDamage* splashDmg = nullptr;
	unique_ptr<DamageInformationBase> Damage;
	switch (dmg->GetType())
	{
	case EDamageType::Single:
		singleDmg = dynamic_cast<SingleDamage*>(dmg);
		Damage = make_unique<SingleDamage>(*singleDmg);
		break;
	case EDamageType::Splash:
		splashDmg = dynamic_cast<SplashDamage*>(dmg);
		Damage = make_unique<SplashDamage>(*splashDmg);
		break;
	}

	m_qDamageInfor.push(move(Damage));
}
void Unit::ReceiveDamage()
{
	while (!m_qDamageInfor.empty())
	{

		SingleDamage* singleDmg = nullptr;
		SplashDamage* splashDmg = nullptr;
		switch (m_qDamageInfor.front()->GetType())
		{
		case EDamageType::Single:
			singleDmg = dynamic_cast<SingleDamage*>(m_qDamageInfor.front().get());
			ReceiveSingleDamageProcess(singleDmg);
			break;
		case EDamageType::Splash:
			splashDmg = dynamic_cast<SplashDamage*>(m_qDamageInfor.front().get());
			ReceiveSplashDamageProcess(splashDmg);
			break;
		}


		m_qDamageInfor.pop();
	}

}
void Unit::ReceiveSingleDamageProcess(SingleDamage* dmg)
{

}
void Unit::ReceiveSplashDamageProcess(SplashDamage* dmg)
{


}




