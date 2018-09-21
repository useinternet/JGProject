#include "TestGround.h"
#include"../EngineFrameWork/Components/Box2DCollisionComponent.h"
TestGround::TestGround()
{
	Ground1 = RegisterComponentInObject<Box2DCollisionComponent>(TT("Ground1"));
	Ground2 = RegisterComponentInObject<Box2DCollisionComponent>(TT("Ground2"));
	Ground3 = RegisterComponentInObject<Box2DCollisionComponent>(TT("Ground3"));
	Ground1->SetAsBox(900.0f, 10.0f);
	Ground2->SetAsBox(10.0F, 500.0f);
	Ground3->SetAsBox(10.0F, 500.0f);
	Ground1->SetBodyType(E2DBodyType::Static);
	Ground2->SetBodyType(E2DBodyType::Static);
	Ground3->SetBodyType(E2DBodyType::Static);
	Ground1->SetComponentLocation(960.0f, 900.0f);
	Ground2->SetComponentLocation(100.0f, 500.0f);
	Ground3->SetComponentLocation(1800.0f, 500.0f);
}

TestGround::~TestGround()
{
}
