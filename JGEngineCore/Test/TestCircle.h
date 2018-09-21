#pragma once
#include"../EngineFrameWork/Object/ExistObject.h"



class TestCircle : public ExistObject
{
private:
	class Circle2DCollisionComponent* Circle;
	//class Box2DCollisionComponent* Box;
public:
	TestCircle();
	virtual ~TestCircle();

	virtual void BeginObject(World* world) override;
	virtual void Tick(const float DeltaTime) override;
	int count = 0;
	std::wstring text= TT("0");
};