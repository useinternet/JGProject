#include"TestObject.h"
#include"../EngineFrameWork/Components/StaticMesh2DComponent.h"
#include"../EngineStatics/JGConstructHelper.h"
#include"../RenderSystem/JGMaterial/JG2DMesh.h"
#include"../EngineFrameWork/Components/TextComponent.h"

//이밋
#include"../EngineFrameWork/2D/Text/JGFontLoader.h"
#include"../EngineFrameWork/Components/InputComponent.h"
#include"../EngineStatics/JGLog.h"
#include"../EngineFrameWork/Components/AnimationMesh2DComponent.h"
#include"TestAnim.h"
#include"../EngineFrameWork/Components/SoundComponent.h"
#include"../EngineStatics/JGSuperClass.h"
#include"../EngineFrameWork/2D/Widget/ImageBox.h"
#include"../EngineFrameWork/World/World.h"
#include"../PhysicsSystem/JGBox2D/JGPhysicsSystem.h"
#include"../PhysicsSystem/JGBox2D/JGDynamics/JG2DBody.h"
#include"../PhysicsSystem/JGBox2D/JGShape/JGPolygonShape.h"
#include"../EngineFrameWork/Components/Box2DCollisionComponent.h"
#include"../PhysicsSystem/JGBox2D/JGShape/JGCircle.h"
using namespace std;
TestObject::TestObject()
{
	RegisterObjectID(typeid(this));

	TestAnimation = RegisterComponentInObject<TestAnim>(TT("Animation"));
	TestAnimation->SetCurrentState(AnimationState::Idle);

	Frame = RegisterComponentInObject<TextComponent>(TT("Text"));
	static JGConstructHelper::TextFont FrameMesh("../ManagementFiles/Resource/Font/Godic.fnt",
		TT("../ManagementFiles/Resource/Font/Godic_0.png"));
	if (FrameMesh.Success)
	{
		Frame->SetConstructObject(FrameMesh.Object);
	}
	Frame->SetTextColor(1.0f, 0.0f, 0.0f);

	MousePosText = RegisterComponentInObject<TextComponent>(TT("MousePos"));
	static JGConstructHelper::TextFont MousePosMesh("../ManagementFiles/Resource/Font/Godic.fnt",
		TT("../ManagementFiles/Resource/Font/Godic_0.png"));
	if (MousePosMesh.Success)
	{
		MousePosText->SetConstructObject(MousePosMesh.Object);
	}


	TestAnimationPosText = RegisterComponentInObject<TextComponent>(TT("TestAnimationPosText"));
	static JGConstructHelper::TextFont TestAnimationPosTextMesh("../ManagementFiles/Resource/Font/Godic.fnt",
		TT("../ManagementFiles/Resource/Font/Godic_0.png"));
	if (TestAnimationPosTextMesh.Success)
	{
		TestAnimationPosText->SetConstructObject(MousePosMesh.Object);
	}
	TestAnimationPosText->SetComponentLocation(0.0f, 50.0f);


	// 입력 실험
	Input = RegisterComponentInObject<InputComponent>(TT("SampleInput"));


	image = RegisterComponentInObject<ImageBox>(TT("SampleImageBox"));
	image->CreateImage(TT("../ManagementFiles/Resource/Breath.png"), EPivot::TopLeft);




	// 물리 실험
	BoxComponent = RegisterComponentInObject<Box2DCollisionComponent>(TT("SampleCollision"));
	BoxComponent->SetAsBox(50.0f, 50.0f);
	BoxComponent->SetDensity(100.0f);
	BoxComponent->SetComponentLocation(800.0f, 10.0f);
	BoxComponent->AddChild(TestAnimation);
}
TestObject::~TestObject()
{

}
void TestObject::BeginObject(World* world)
{
	ExistObject::BeginObject(world);
	Frame->SetComponentLocation(300.0f, 500.0f);
	Input->BindKeyCommand(TT("Right"), EKeyState::Down,
		bind(&TestObject::Right, this));
	Input->BindKeyCommand(TT("Left"), EKeyState::Down,
		bind(&TestObject::Left, this));
	Input->BindKeyCommand(TT("Up"), EKeyState::Down,
		bind(&TestObject::Up, this));
	Input->BindKeyCommand(TT("Down"), EKeyState::Down,
		bind(&TestObject::Down, this));
	JGVector2D vec(960.0f, 900.0f);
	JGAngle2D angle(0.0f);
	JG2DBody* body = GetWorld()->GetPyWorld()->Create2DBody(E2DBodyType::Static,&vec, &angle);
	JGPolygonShape shape;
	shape.SetAsBox(900.0f, 10.0f);
	JGCircle circle;
	circle.SetCircle(50.0f);
	body->CreateFixture(nullptr,&shape);


	vec.Set(960.0f, 0.0f);
	angle.Set(0.0f);
	JG2DBody* body2 = GetWorld()->GetPyWorld()->Create2DBody(E2DBodyType::Dynamic, &vec, &angle);
	shape.SetAsBox(50.0f, 50.0f);
	body2->CreateFixture(nullptr,&circle, 1.0f,0.3f,0.3f);


	BoxComponent->GetBody()->PhysicsOff();
}

void TestObject::Tick(const float DeltaTime)
{
	ExistObject::Tick(DeltaTime);
	TestAnimation->AddComponentAngle(DeltaTime * 10);
	// 임시 프레임 알아보기
	float FPS = 1.0f / DeltaTime;
	Frame->SetText(TT("FPS : %d"), (int)FPS);
	JGVector2D animPos = TestAnimation->GetComponentWorldLocation();
	JGVector2D MousePos = GetRenderSuperClass()->GetCommandManager()->GetMouseLocation();
	image->AddBlend(DeltaTime * (-0.3f));
	TestAnimationPosText->SetText(TT(" X : %d  ,  Y : %d  "), (int)animPos.X(), (int)animPos.Y());
	MousePosText->SetText(TT("Mouse X : %d , Mouse Y : %d"), (int)MousePos.X(), (int)MousePos.Y());




	JGVector2D gravity = GetWorld()->GetPyWorld()->GetGravity();
	JGVector2D vel = BoxComponent->GetBody()->GetLinearVelocity();
	float velChangeX = BoxComponent->GetVelocity().X() - vel.X();
	JGVector2D Impulse(BoxComponent->GetBody()->GetMass() * velChangeX,0.0f);
	BoxComponent->GetBody()->ApplyLinearImpulse(Impulse);
}
void TestObject::Right()
{
	JGVector2D vel(100.0f, 0.0f);
	BoxComponent->SetVelocity(vel);
	BoxComponent->AddComponentLocation(1.0f, 0.0f);
	TestAnimation->SetCurrentState(AnimationState::Right);

}
void TestObject::Left()
{
	JGVector2D vel(-100.0f, 0.0f);
	BoxComponent->SetVelocity(vel);
	BoxComponent->AddComponentLocation(-1.0f, 0.0f);
	TestAnimation->SetCurrentState(AnimationState::Left);
}
void TestObject::Up()
{
	JGVector2D Impulse(0.0f, BoxComponent->GetBody()->GetMass() * -10.0f);
	BoxComponent->GetBody()->ApplyLinearImpulse(Impulse);
	BoxComponent->AddComponentLocation(0.0f, -1.0f);
	TestAnimation->SetCurrentState(AnimationState::Up);
}
void TestObject::Down()
{
	BoxComponent->AddComponentLocation(0.0f, 1.0f);
	TestAnimation->SetCurrentState(AnimationState::Down);
}





