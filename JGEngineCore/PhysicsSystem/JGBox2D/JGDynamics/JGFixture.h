#pragma once
#include"../../../EngineStatics/Engine.h"
#include"../../Box2D/Box2D.h"
class JG2DBody;
class ENGINE_EXPORT JGFixture
{
private:
	friend JG2DBody;
private:
	b2Fixture* m_Fixture = nullptr;

public:
	JGFixture();
	~JGFixture();

	b2Fixture* Get();
	
private:
	void CreateFixture(b2Fixture* fixture);
	void Set(b2Fixture* fixture);
};