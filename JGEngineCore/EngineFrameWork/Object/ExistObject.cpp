#include"ExistObject.h"
#include"../Components/StaticMesh2DComponent.h"
#include"../../EngineStatics/JGConstructHelper.h"
#include"../../RenderSystem/JGMaterial/JG2DMesh.h"
#include"../Components/TextComponent.h"

//이밋
#include"../2D/Text/JGFontLoader.h"
#include"../Components/InputComponent.h"
#include"../../EngineStatics/JGLog.h"
#include"../Components/AnimationMesh2DComponent.h"
#include"../../Test/TestAnim.h"
#include"../Components/SoundComponent.h"
#include"../2D/Widget/ImageBox.h"
#include"../../EngineStatics/JGSuperClass.h"
#include"../2D/Widget/Button.h"
#include"../../RenderSystem/JGViewportD.h"
#include"../Components/Box2DCollisionComponent.h"
using namespace std;
ExistObject::ExistObject()
{
	RegisterObjectID(typeid(this));


	Ground = RegisterComponentInObject<Box2DCollisionComponent>(TT("Ground"));
	Ground->SetComponentLocation(960.0f, 880.0f);
	Ground->SetBox(900.0f, 50.0f);
	Ground->DebugRenderingOn();


	SampleCollison = RegisterComponentInObject<Box2DCollisionComponent>(TT("SampleCollison"));
	SampleCollison->SetBox(50.0f, 50.0f);
	SampleCollison->SetCollisionType(E2DBodyType::Dyanamic);
	SampleCollison->DebugRenderingOn();
	SampleCollison->SetComponentLocation(200.f, 0.0f);


	TestAnimation = RegisterComponentInObject<TestAnim>(TT("Animation"));
	TestAnimation->SetCurrentState(AnimationState::Idle);
	SampleCollison->AddChild(TestAnimation);

	Frame = RegisterComponentInObject<TextComponent>(TT("Text"));
	static JGConstructHelper::TextFont FrameMesh(
		GetDevice(), "../ManagementFiles/Resource/Font/Godic.fnt",
		TT("../ManagementFiles/Resource/Font/Godic_0.png"));
	if (FrameMesh.Success)
	{
		Frame->SetConstructObject(FrameMesh.Object);
	}
	Frame->SetTextColor(1.0f, 0.0f, 0.0f);


	MousePosText = RegisterComponentInObject<TextComponent>(TT("MousePos"));
	static JGConstructHelper::TextFont MousePosMesh(
		GetDevice(), "../ManagementFiles/Resource/Font/Godic.fnt",
		TT("../ManagementFiles/Resource/Font/Godic_0.png"));
	if (MousePosMesh.Success)
	{
		MousePosText->SetConstructObject(MousePosMesh.Object);
	}


	TestAnimationPosText = RegisterComponentInObject<TextComponent>(TT("TestAnimationPosText"));
	static JGConstructHelper::TextFont TestAnimationPosTextMesh(
		GetDevice(), "../ManagementFiles/Resource/Font/Godic.fnt",
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
}
ExistObject::~ExistObject()
{


}
void ExistObject::BeginObject(World* world)
{
	Object::BeginObject(world);
	Frame->SetComponentLocation(300.0f, 500.0f);
	Input->BindKeyCommand(TT("Right"), EKeyState::Down,
		bind(&ExistObject::Right, this));
	Input->BindKeyCommand(TT("Left"), EKeyState::Down,
		bind(&ExistObject::Left, this));
	Input->BindKeyCommand(TT("Up"), EKeyState::Down,
		bind(&ExistObject::Up, this));
	Input->BindKeyCommand(TT("Down"), EKeyState::Down,
		bind(&ExistObject::Down, this));
}

void ExistObject::Tick(const float DeltaTime)
{
	Object::Tick(DeltaTime);
	TestAnimation->AddComponentAngle(DeltaTime * 10);
	// 임시 프레임 알아보기
	float FPS = 1.0f / DeltaTime;
	Frame->SetText(TT("FPS : %d"), (int)FPS);
	JGVector2D animPos = TestAnimation->GetComponentWorldLocation();
	JGVector2D MousePos = GetRenderSuperClass()->GetCommandManager()->GetMouseLocation();
	image->AddBlend(DeltaTime * (-0.3f));
	JGVector2D vec = SampleCollison->GetComponentWorldLocation();
	TestAnimationPosText->SetText(TT(" X : %d  ,  Y : %d  "), (int)animPos.X(), (int)animPos.Y());
	MousePosText->SetText(TT("Mouse X : %d , Mouse Y : %d"), (int)vec.X(), (int)vec.Y());
}
void ExistObject::Right()
{
	SampleCollison->AddComponentLocation(1.0f, 0.0f);
	TestAnimation->SetCurrentState(AnimationState::Right);
}
void ExistObject::Left()
{
	SampleCollison->AddComponentLocation(-1.0f, 0.0f);
	TestAnimation->SetCurrentState(AnimationState::Left);
}
void ExistObject::Up()
{
	SampleCollison->AddComponentLocation(0.0f, -1.0f);
	TestAnimation->SetCurrentState(AnimationState::Up);
}
void ExistObject::Down()
{
	SampleCollison->AddComponentLocation(0.0f, 1.0f);
	TestAnimation->SetCurrentState(AnimationState::Down);
}





