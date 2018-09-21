#include"JGContactListener.h"
#include"../../../EngineFrameWork/Components/CollisionComponent.h"

JGContactListener::JGContactListener()
{
}

JGContactListener::~JGContactListener()
{
}

void JGContactListener::BeginContact(b2Contact* contact)
{
	void* DataA = contact->GetFixtureA()->GetBody()->GetUserData();
	void* DataB = contact->GetFixtureB()->GetBody()->GetUserData();
	if (DataA == nullptr || DataB == nullptr)
	{
		return;
	}
	CollisionComponent* CollisionDataA = static_cast<CollisionComponent*>(DataA);
	CollisionComponent* CollisionDataB = static_cast<CollisionComponent*>(DataB);
	

}

void JGContactListener::EndContact(b2Contact* contact)
{

}

void JGContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
}

void JGContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
}
