#pragma once

#include"JGShape.h"
class JGVector2D;

class ENGINE_EXPORT JGPolygonShape : public JGShape
{
private:
	std::unique_ptr<b2PolygonShape> m_Shape;

public:
	JGPolygonShape();
	virtual ~JGPolygonShape();
	void SetAsBox(const float HalfWidth, const float HalfHeight);
	void SetAsBox(const float HalfWidth, const float HalfHeight, const JGVector2D& center);
};