#include"TestCircle.h"
#include"../EngineFrameWork/World/World.h"
#include"../PhysicsSystem/JGBox2D/JGShape/JGCircle.h"
#include"../PhysicsSystem/JGBox2D/JGDynamics/JGPhysicsWorld.h"
#include"../PhysicsSystem/JGBox2D/JGDynamics/JG2DBody.h"
#include"../EngineFrameWork/Components/Circle2DCollisionComponent.h"
#include"../EngineFrameWork/Components/Box2DCollisionComponent.h"
#include"TestObject.h"
#include"../EngineStatics/JGLog.h"
TestCircle::TestCircle()
{
	static std::wstring s = TT("충돌 : %d 원 1");
	text = s;
	s += TT("1");
	Circle = RegisterComponentInObject<Circle2DCollisionComponent>(TT("CircleComponent"));
	Circle->SetComponentLocation(900.0f, 0.0f);
	Circle->SetRadius(50.0f);
	Circle->SetRestitution(0.0f);

	//Box = RegisterComponentInObject<Box2DCollisionComponent>(TT("BoxComponent"));
	//Box->SetAsBox(50.0f, 50.0f);
	//Box->SetComponentLocation(900.0f, 0.0f);
}

TestCircle::~TestCircle()
{
}

void TestCircle::BeginObject(World * world)
{
	ExistObject::BeginObject(world);
	Circle->SetBeginOverlapEvent([world](Object* beginObject)
	{

		TestObject* ob = dynamic_cast<TestObject*>(beginObject);
		if (ob)
		{
			static int count = 0;
			count++;
			JGLog::Write(ELogLevel::Default, TT("카운트 : %d "), count);
			world->SpawnObject<TestCircle>();
			//this->count += 1;
			//JGLog::Write(ELogLevel::Default, this->text.c_str(), this->count);
		}

	});
}

void TestCircle::Tick(const float DeltaTime)
{
	ExistObject::Tick(DeltaTime);
}
