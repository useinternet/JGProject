#pragma once
#include"Object.h"

/*
Class : ExistObject 
Exp : 화면상에 그려지는 오브젝트.. */
class ExistObject : public Object
{
private:
	class TextComponent* Frame;
	class InputComponent* Input;
	class TestAnim* TestAnimation;
	class SoundComponent* sound;
	class ImageBox* image;
public:
	ExistObject();
	virtual ~ExistObject();

	virtual void BeginObject() override;

	virtual void Tick(const float DeltaTime) override;
	void Right();
	void Left();
	void Up();
	void Down();
};

