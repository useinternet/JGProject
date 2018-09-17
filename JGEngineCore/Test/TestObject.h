#pragma once
#include"../EngineFrameWork/Object/ExistObject.h"


class ENGINE_EXPORT TestObject : public ExistObject
{
private:
	class TextComponent* Frame;
	class InputComponent* Input;
	class TestAnim* TestAnimation;
	class ImageBox* image;
	class TextComponent* MousePosText;
	class TextComponent* TestAnimationPosText;
public:
	TestObject();
	virtual ~TestObject();

	virtual void BeginObject(World* world) override;

	virtual void Tick(const float DeltaTime) override;
	void Right();
	void Left();
	void Up();
	void Down();
};