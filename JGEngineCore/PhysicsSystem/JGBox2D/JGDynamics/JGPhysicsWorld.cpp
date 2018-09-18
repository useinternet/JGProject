#include"JGPhysicsWorld.h"
#include"../../../EngineStatics/JMath/JGVector2D.h"
#include"../../../EngineStatics/JMath/JGAngle2D.h"

#include"../JGCommon/b2ToJGCommon.h"
#include"JG2DBody.h"
using namespace std;
b2BodyDef JGPhysicsWorld::m_BodyDef;
JGPhysicsWorld::JGPhysicsWorld(b2Draw* DebugDraw, JGVector2D& Gravity)
{
	m_World = make_unique<b2World>(JGTob2_Force(Gravity));
	m_World->SetDebugDraw(DebugDraw);
}
JGPhysicsWorld::~JGPhysicsWorld()
{
	for (auto& iter : m_vJGBody)
	{
		m_World->DestroyBody(iter->m_Body);
	}
}
JG2DBody* JGPhysicsWorld::Create2DBody(const E2DBodyType type, JGVector2D* LinkLocation, JGAngle2D * LinkAngle)
{
	JG2DBody* result = nullptr;
	switch (type)
	{
	case E2DBodyType::Dynamic:
		m_BodyDef.type = b2_dynamicBody;
		break;
	case E2DBodyType::Static:
		m_BodyDef.type = b2_staticBody;
		break;
	case E2DBodyType::Kinematic:
		m_BodyDef.type = b2_kinematicBody;
		break;
	}
	b2Vec2 vec;
	if (LinkLocation)
	{
		vec = JGTob2_Vector(*LinkLocation);
	}
	else
	{
		vec.Set(0.0f, 0.0f);
	}
	m_BodyDef.position = vec;
	if (LinkAngle)
	{
		m_BodyDef.angle = ToRadian(LinkAngle->Get());
	}
	else
	{
		m_BodyDef.angle = 0.0f;
	}

	unique_ptr<JG2DBody> body = make_unique<JG2DBody>();
	result = body.get();
	b2Body* Box2D_body = m_World->CreateBody(&m_BodyDef);
	body->CreateBody(Box2D_body, LinkLocation, LinkAngle);
	m_vJGBody.push_back(move(body));

	return result;
}

void JGPhysicsWorld::DestoryBody(JG2DBody* body)
{
	body->DestroyBody();
}

void JGPhysicsWorld::Step(const float DeltaTime, int32 velocityIterations, int32 positionIterations)
{
	for (auto iter = m_vJGBody.begin(); iter < m_vJGBody.end();)
	{
		if ((*iter)->GetBodyState() == E2DBodyState::Destory)
		{
			m_World->DestroyBody((*iter)->m_Body);
			iter = m_vJGBody.erase(iter);
		}
		else
		{
			(*iter)->Tick();
			++iter;
		}
	}
	m_World->Step(DeltaTime, velocityIterations, positionIterations);
}

void JGPhysicsWorld::DebugRender()
{
	m_World->DrawDebugData();
}
