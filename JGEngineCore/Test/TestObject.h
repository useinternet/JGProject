#pragma once
#include"../EngineFrameWork/Object/ExistObject.h"

#include"../PhysicsSystem/JGBox2D/JGDynamics/JGPhysicsWorld.h"
#include"../PhysicsSystem/JGBox2D/JGDynamics/JG2DBody.h"
#include"../PhysicsSystem/JGBox2D/JGDynamics/JGFixture.h"
#include"../PhysicsSystem/JGBox2D/JGShape/JGPolygonShape.h"

class ENGINE_EXPORT TestObject : public ExistObject
{
private:
	class TextComponent* Frame;
	class InputComponent* Input;
	class TestAnim* TestAnimation;
	class ImageBox* image;
	class TextComponent* MousePosText;
	class TextComponent* TestAnimationPosText;
	class Box2DCollisionComponent* BoxComponent;
	bool sample = false;
	bool PrevSample = false;
	// 물리 실험..
public:
	TestObject();
	virtual ~TestObject();

	virtual void BeginObject(World* world) override;

	virtual void Tick(const float DeltaTime) override;
	void Right();
	void Left();
	void Up();
	void Down();
	void Spawn();
};