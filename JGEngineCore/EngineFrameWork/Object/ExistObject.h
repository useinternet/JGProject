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
	class StaticMesh2DComponent* Ground;




	std::unique_ptr<b2World> sampleworld;
	std::unique_ptr<b2BodyDef> GroundBodyDef;
	b2Body* GroundBody = nullptr;
	std::unique_ptr<b2PolygonShape> GroundBox;

	std::unique_ptr<b2BodyDef> AnimBodyDef;
	b2Body* AnimBody;
	std::unique_ptr<b2PolygonShape> AnimBodyBox;
	std::unique_ptr<b2FixtureDef> fixtureDef;
	b2Fixture* fixture;
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

