#pragma once
#include"../../../EngineStatics/Engine.h"
#include"../../Box2D/Box2D.h"


class ENGINE_EXPORT JG2DBody
{
private:
	b2Body* m_Body = nullptr;

public:
	JG2DBody();
	~JG2DBody();
};