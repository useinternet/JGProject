#pragma once
#include"Object.h"



// 점프력
// 속도
// 피벗에 따른 디버그 렌더링
// 중심 좌표


class PhysicsWorld;
/*
Class : ExistObject 
Exp : 화면상에 그려지는 오브젝트.. */
class ExistObject : public Object
{
private:
	class TextComponent* Frame;
	class InputComponent* Input;
	class TestAnim* TestAnimation;

	class Box2DCollisionComponent* SampleCollison;
	class ImageBox* image;
	class Box2DCollisionComponent* Ground;
	class TextComponent* MousePosText;
	class TextComponent* TestAnimationPosText;
	std::unique_ptr<PhysicsWorld> world;
public:
	ExistObject();
	virtual ~ExistObject();

	virtual void BeginObject(World* world) override;

	virtual void Tick(const float DeltaTime) override;
	void Right();
	void Left();
	void Up();
	void Down();
};

