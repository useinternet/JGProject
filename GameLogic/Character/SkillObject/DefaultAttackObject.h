#pragma once
#include"EngineFrameWork/Object/ExistObject.h"


class DefaultAttackObject : public ExistObject
{
private:
	class Circle2DCollisionComponent* CircleCollison;
	float ObjectLife = 10.0f;
	class JTimerHandle* DestroyTimerHandle = nullptr;
	float m_VelScale = 1.0f;
public:
	DefaultAttackObject();
	virtual ~DefaultAttackObject();
	virtual void BeginObject(World* world) override;
	virtual void Tick(const float DeltaTime);
	void SetAttackSpawnLocation(const float x, const float y,const float VelScale);
	void SetAttackSpawnLocation(const JGVector2D& vec, const float VelScale);
};