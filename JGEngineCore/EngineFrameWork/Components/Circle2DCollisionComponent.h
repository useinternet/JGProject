#pragma once
#include"CollisionComponent.h"

class JGCircle;
class ENGINE_EXPORT Circle2DCollisionComponent : public CollisionComponent
{
private:
	std::unique_ptr<JGCircle> m_Circle;
	float m_Radius = 50.0f;

public:
	Circle2DCollisionComponent();
	virtual ~Circle2DCollisionComponent();
	virtual void BeginComponent(World* world) override;
	void SetRadius(const float Radius);
};