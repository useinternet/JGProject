#pragma once
#include"JGShape.h"



class ENGINE_EXPORT JGCircle : public JGShape
{
private:
	std::unique_ptr<b2CircleShape> m_Circle;


public:
	JGCircle();
	virtual ~JGCircle();

	void SetCircle(const float Radius);
};