#include"JG2DBody.h"
#include"JGShape.h"
#include"JGRectangle.h"
#include"PhysicsWorld.h"
#include"../EngineStatics/JMath/JGVector2D.h"
using namespace std;

JG2DBody::JG2DBody()
{

}
JG2DBody::~JG2DBody()
{

}
void JG2DBody::CreateBody(PhysicsWorld* world, JGShape* shape, const E2DBodyType type, const SJG2DBodyDesc& BodyDesc)
{
	m_BodyDesc = make_unique<SJG2DBodyDesc>();
	*m_BodyDesc = BodyDesc;
	m_pShape = shape;
	m_Type = type;
	m_pWorld = world;
}
void JG2DBody::Tick(const float DeltaTime)
{
	if (m_pShape)
	{
		m_Acceleration = m_pWorld->GetGravity() * DeltaTime;
		m_Velocity += m_Acceleration;
		m_pShape->Tick();
		JGVector2D vec(0.0f, m_Velocity);
		m_pShape->AddLocation(vec);
	}
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
	return m_Velocity;
}
float JG2DBody::GetAcceleration()
{
	return m_Acceleration;
}
