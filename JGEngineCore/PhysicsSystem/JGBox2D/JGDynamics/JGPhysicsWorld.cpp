#include"JGPhysicsWorld.h"
#include"../JGCommon/JGCollisionDebugDraw.h"
#include"JG2DBody.h"
using namespace std;
JGPhysicsWorld::JGPhysicsWorld()
{
	//m_World = make_unique<b2World>();
}
JGPhysicsWorld::~JGPhysicsWorld()
{

}

JG2DBody * JGPhysicsWorld::Create2DBody()
{
	return nullptr;
}

void JGPhysicsWorld::Step(const float DeltaTime, int32 velocityIterations, int32 positionIterations)
{
}
