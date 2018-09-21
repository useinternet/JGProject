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
	void* DataA = contact->GetFixtureA()->GetUserData();
	void* DataB = contact->GetFixtureB()->GetUserData();
	if (DataA == nullptr || DataB == nullptr)
	{
		return;
	}
	CollisionComponent* CollisionDataA = static_cast<CollisionComponent*>(DataA);
	CollisionComponent* CollisionDataB = static_cast<CollisionComponent*>(DataB);
	
	CollisionDataA->AddOverlapObject(CollisionDataB->GetOwnerObject());
	CollisionDataB->AddOverlapObject(CollisionDataA->GetOwnerObject());
}

void JGContactListener::EndContact(b2Contact* contact)
{
	void* DataA = contact->GetFixtureA()->GetUserData();
	void* DataB = contact->GetFixtureB()->GetUserData();
	if (DataA == nullptr || DataB == nullptr)
	{
		return;
	}
	CollisionComponent* CollisionDataA = static_cast<CollisionComponent*>(DataA);
	CollisionComponent* CollisionDataB = static_cast<CollisionComponent*>(DataB);

	CollisionDataA->DeleteOverlapObject(CollisionDataB->GetOwnerObject());
	CollisionDataB->DeleteOverlapObject(CollisionDataA->GetOwnerObject());
}

void JGContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
}

void JGContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
}
