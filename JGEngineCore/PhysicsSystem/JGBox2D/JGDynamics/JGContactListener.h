#pragma once
#include"../../../EngineStatics/Engine.h"
#include"../../Box2D/Box2D.h"


class JGContactListener : public b2ContactListener
{
private:



public:
	JGContactListener();
	virtual ~JGContactListener();

	virtual void BeginContact(b2Contact* contact) override;
	virtual void EndContact(b2Contact* contact) override;
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
};