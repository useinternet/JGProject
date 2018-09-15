#include"JG2DBody.h"
#include"../EngineStatics/JMath/JGVector2D.h"
#include"JGShape/JGShape.h"
#include"JGShape/JGRectangle.h"
#include"PhysicsWorld.h"
#include"JGCollisionCheckManager/JGCollisionCheck.h"
#include"JGCollisionCheckManager/JGCollisionCheckManager.h"
#include"PhysicsCalculationSystem.h"
#include"../EngineStatics/JMath/JGVector2D.h"
using namespace std;

JG2DBody::JG2DBody()
{
	m_Force = make_unique<JGVector2D>();
	m_AccForce = make_unique<JGVector2D>();
}
JG2DBody::~JG2DBody()
{

}
JGCollisionCheck* JG2DBody::CreateCollisionChecker()
{
	m_CollisionChecker = make_unique<JGCollisionCheck>();
	m_CollisionChecker->LinkBody(this);
	return m_CollisionChecker.get();
}
JGCollisionCheck* JG2DBody::GetCollisionChecker()
{
	return m_CollisionChecker.get();
}
JGShape* JG2DBody::GetShape()
{
	return m_pShape;
}
E2DBodyType JG2DBody::GetType()
{
	return m_Type;
}
float JG2DBody::GetVelocity()
{
	return m_Force->LengthSquard();
}
float JG2DBody::GetAcceleration()
{
	return m_AccForce->LengthSquard();
}

JGVector2D& JG2DBody::GetForce()
{
	return *m_Force;
}

void JG2DBody::CreateBody(PhysicsWorld* world, JGShape* shape, const E2DBodyType type)
{
	m_pShape = shape;
	m_Type = type;
	m_pWorld = world;
}
void JG2DBody::AddForce(const float x, const float y)
{
	JGVector2D vec(x, y);
	*m_Force += vec;
}
void JG2DBody::AddForce(const JGVector2D& vec)
{
	JGVector2D vecter(vec);
	*m_Force += vecter;
}
void JG2DBody::AddForceX(const float x)
{
	JGVector2D vec(x, 0.0f);
	*m_Force += vec;
}
void JG2DBody::AddForceY(const float y)
{
	JGVector2D vec(0.0f, y);
	*m_Force += vec;
}
void JG2DBody::SetForce(const float x, const float y)
{
	m_Force->Set(x, y);
}
void JG2DBody::SetForce(const JGVector2D& vec)
{
	m_Force->Set(vec);
}
void JG2DBody::Tick(const float DeltaTime)
{
	if (m_CollisionChecker.get())
	{
		m_CollisionChecker->Tick();
	}
	if (m_pShape)
	{
		m_pShape->Tick();
	}
}
