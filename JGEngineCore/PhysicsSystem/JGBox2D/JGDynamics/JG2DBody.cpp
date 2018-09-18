#include"JG2DBody.h"
#include"JGPhysicsWorld.h"
#include"JGFixture.h"
#include"../JGShape/JGShape.h"
#include"../../../EngineStatics/JMath/JGVector2D.h"
#include"../../../EngineStatics/JMath/JGAngle2D.h"
#include"../../../EngineStatics/JGLog.h"
using namespace std;
b2FixtureDef JG2DBody::m_FixtureDef;

JG2DBody::JG2DBody()
{
	m_BodyState = E2DBodyState::Active;
}
JG2DBody::~JG2DBody()
{

}

JGFixture* JG2DBody::CreateFixture(JGShape* shape, const float friction, 
	const float restitution, const float density, const bool isSensor)
{
	JGFixture* result = nullptr;
	unique_ptr<JGFixture> fixture = make_unique<JGFixture>();

	m_FixtureDef.shape = shape->Get();
	m_FixtureDef.friction = friction;
	m_FixtureDef.isSensor = isSensor;
	m_FixtureDef.restitution = restitution;
	m_FixtureDef.density = density;
	// 나중에 필터 작업..
	//m_FixtureDef.filter
	fixture->Set(m_Body->CreateFixture(&m_FixtureDef));

	result = fixture.get();
	m_vFixtures.push_back(move(fixture));

	return result;
}

E2DBodyState JG2DBody::GetBodyState()
{
	return m_BodyState;
}

void JG2DBody::Tick()
{
	static int num = 0;
	num++;
	if (num > 1000)
	{
		num = 0;
		JGLog::Write(ELogLevel::Default, TT("x : %d , y : %d"),(int)m_Body->GetPosition().x, (int)m_Body->GetPosition().y);
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

