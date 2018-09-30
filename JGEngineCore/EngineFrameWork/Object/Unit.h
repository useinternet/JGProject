#pragma once
#include"../../EngineStatics/Engine.h"
#include"ExistObject.h"


class ENGINE_EXPORT Unit : public ExistObject
{
private:
	enum FallingCheck
	{
		None,
		Down,
		Up
	};
	enum DirectionCheck
	{
		Right,
		Left
	};
private:
	class Box2DCollisionComponent* m_Collision;
	FallingCheck   m_FallingCheck;
	DirectionCheck m_CurrentDirection;
	bool m_bStill = false;
public:
	Unit();
	virtual ~Unit();
	virtual void BeginObject(World* world) override;
	virtual void Tick(const float DeltaTime) override;
	Box2DCollisionComponent* GetCollision();


	bool IsRight();
	bool IsLeft();
	bool IsStill();
	bool IsFalling();
	bool IsFallingDown();
	bool IsFallingUp();
};