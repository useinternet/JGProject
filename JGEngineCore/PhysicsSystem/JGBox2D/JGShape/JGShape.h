#pragma once
#include"../../../EngineStatics/Engine.h"
#include"../../Box2D/Box2D.h"


class ENGINE_EXPORT JGShape
{
private:
	b2Shape* m_Shape = nullptr;

public:
	JGShape();
	virtual ~JGShape();
	b2Shape* Get();

protected:
	void RegisterShapeOrigin(b2Shape* shape);
};