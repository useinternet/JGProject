#pragma once
#include"../EngineStatics/Engine.h"
class JGVector2D;
class ENGINE_EXPORT Figure
{
public:
	virtual ~Figure();
};
class ENGINE_EXPORT Box : public Figure
{
public:
	float Top = 0.0f;
	float Left = 0.0f;
	float Right = 0.0f;
	float Bottom = 0.0f;

	void Set(const float top, const float left, const float right, const float bottom);
	void Set(JGVector2D& LeftTop, JGVector2D& RightBottom);
	virtual ~Box();
};

class ENGINE_EXPORT Circle : public Figure
{
public:
	float cX = 0.0f;
	float cY = 0.0f;
	float radius = 0.0f;
	virtual ~Circle();
};

/*
추후 구현..*/
class ENGINE_EXPORT Capsule : public Figure
{
public:
	virtual ~Capsule();
};