#pragma once
#include"Object.h"



// 점프력
// 속도
// 피벗에 따른 디버그 렌더링
// 중심 좌표


/*
Class : ExistObject 
Exp : 화면상에 그려지는 오브젝트.. */
class ExistObject : public Object
{
private:
	class TextComponent* Frame;
	class InputComponent* Input;
	class TestAnim* TestAnimation;

	class ImageBox* image;
	class TextComponent* MousePosText;
	class TextComponent* TestAnimationPosText;
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

