#include"JG2DBody.h"
#include"JGPhysicsWorld.h"
#include"JGFixture.h"
#include"../JGShape/JGShape.h"
#include"../../../EngineStatics/JMath/JGVector2D.h"
#include"../../../EngineStatics/JMath/JGAngle2D.h"
#include"../JGCommon/b2ToJGCommon.h"
#include"../../../EngineFrameWork/Components/CollisionComponent.h"
using namespace std;
b2FixtureDef JG2DBody::m_FixtureDef;

JG2DBody::JG2DBody()
{
	m_BodyState = E2DBodyState::Active;

}
JG2DBody::~JG2DBody()
{

}

JGFixture* JG2DBody::CreateFixture(CollisionComponent* Collision,JGShape* shape, const float density,
	const float friction, const float restitution, const bool isSensor)
{
	JGFixture* result = nullptr;
	unique_ptr<JGFixture> fixture = make_unique<JGFixture>();

	m_FixtureDef.shape       = shape->Get();
	m_FixtureDef.friction    = friction;
	m_FixtureDef.isSensor    = isSensor;
	m_FixtureDef.restitution = restitution;
	m_FixtureDef.density     = density;
	m_FixtureDef.userData    = (void*)Collision;
	// 나중에 필터 작업..
	//m_FixtureDef.filter
	fixture->Set(m_Body->CreateFixture(&m_FixtureDef));

	result = fixture.get();
	m_vFixtures.push_back(move(fixture));

	return result;
}

void JG2DBody::PhysicsOff()
{
	m_IsPhysics = false;
	m_Body->SetGravityScale(0.0f);
	m_Body->SetFixedRotation(true);
}

void JG2DBody::PhysicsOn(bool IsFixedRotation)
{
	m_IsPhysics = true;
	m_Body->SetGravityScale(1.0f);
	m_Body->SetFixedRotation(IsFixedRotation);
}

void JG2DBody::SetLinearVelocity(JGVector2D& velocity)
{
	b2Vec2 vec = JGTob2_Force(velocity);
	m_Body->SetLinearVelocity(vec);
}

void JG2DBody::ApplyForce(JGVector2D& force, bool wake)
{
	b2Vec2 b2Force = JGTob2_Force(force);
	m_Body->ApplyForce(b2Force, m_Body->GetWorldCenter(), wake);
}

void JG2DBody::ApplyLinearImpulse(JGVector2D & impulse, bool wake)
{
	b2Vec2 b2Impulse = JGTob2_Force(impulse);
	m_Body->ApplyLinearImpulse(b2Impulse, m_Body->GetWorldCenter(), wake);
}

void JG2DBody::ApplyLinearImpulseToCenter(JGVector2D& impulse, bool wake)
{
	b2Vec2 b2Impulse = JGTob2_Force(impulse);
	m_Body->ApplyLinearImpulseToCenter(b2Impulse, wake);
}

float JG2DBody::GetMass()
{
	return (float)m_Body->GetMass();
}

JGVector2D JG2DBody::GetLinearVelocity()
{
	b2Vec2 b2Vel = m_Body->GetLinearVelocity();
	JGVector2D velocity = b2ToJG_Force(b2Vel);
	return velocity;
}

E2DBodyState JG2DBody::GetBodyState()
{
	return m_BodyState;
}
void JG2DBody::SetTransform(JGVector2D& Location, JGAngle2D& Angle)
{
	b2Vec2 b2Location = JGTob2_Vector(Location);
	float RdAngle = ToRadian(Angle.Get());

	m_Body->SetTransform(b2Location, RdAngle);
	m_Body->SetAwake(true);
}

void JG2DBody::Tick()
{
	if (!m_IsPhysics)
	{
		if (m_LinkLocation && m_LinkAngle)
		{
			b2Vec2 vel(0.0f, 0.0f);
			m_Body->SetLinearVelocity(vel);
			SetTransform(*m_LinkLocation, *m_LinkAngle);
			m_Body->SetGravityScale(0.0f);
		}
	}
	else
	{
		if (m_LinkLocation && m_LinkAngle)
		{
			b2Vec2 vec = m_Body->GetPosition();
			m_LinkLocation->Set(b2ToJG_Vector(vec));

			float angle = m_Body->GetAngle();
			m_LinkAngle->Set(ToDegree(angle));
			m_Body->SetGravityScale(1.0f);
		}
	}
	

}

void JG2DBody::CreateBody(b2Body* body, JGVector2D* LinkLocation, JGAngle2D* LinkAngle)
{
	m_Body = body;
	m_LinkLocation = LinkLocation;
	m_LinkAngle = LinkAngle;
}

b2Body* JG2DBody::DestroyBody()
{
	m_LinkLocation = nullptr;
	m_LinkAngle    = nullptr;
	m_BodyState    = E2DBodyState::Destory;
	return m_Body;
}

