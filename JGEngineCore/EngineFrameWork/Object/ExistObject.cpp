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
using namespace std;
ExistObject::ExistObject()
{
	RegisterObjectID(typeid(this));
	Ground = RegisterComponentInObject<StaticMesh2DComponent>(TT("Ground"));

	static JGConstructHelper::StaticMesh2D GroundMesh(
		GetDevice(), GetBufferManager(), Ground->GetComponentName(),
		EPivot::TopLeft, TT("../ManagementFiles/Resource/SampleGround.png"));
	if (GroundMesh.Success)
	{
		Ground->SetConstructObject(GroundMesh.Object);
	}


	TestAnimation = RegisterComponentInObject<TestAnim>(TT("Animation"));
	TestAnimation->SetCurrentState(AnimationState::Idle);


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
void ExistObject::BeginObject()
{
	Object::BeginObject();
	TestAnimation->SetComponentLocation(400.0f, 0.0f);
	Frame->SetComponentLocation(300.0f, 500.0f);
	Input->BindKeyCommand(TT("Right"), EKeyState::Down,
		bind(&ExistObject::Right, this));
	Input->BindKeyCommand(TT("Left"), EKeyState::Down,
		bind(&ExistObject::Left, this));
	Input->BindKeyCommand(TT("Up"), EKeyState::Down,
		bind(&ExistObject::Up, this));
	Input->BindKeyCommand(TT("Down"), EKeyState::Down,
		bind(&ExistObject::Down, this));
	Ground->SetComponentLocation(100.0f, 900.0f);
}

void ExistObject::Tick(const float DeltaTime)
{
	Object::Tick(DeltaTime);

	// 임시 프레임 알아보기
	float FPS = 1.0f / DeltaTime;
	Frame->SetText(TT("FPS : %d"), (int)FPS);
	JGVector2D animPos = TestAnimation->GetComponentWorldLocation();
	JGVector2D MousePos = GetRenderSuperClass()->GetCommandManager()->GetMouseLocation();
	image->AddBlend(DeltaTime * (-0.3f));
	
	b2Vec2 BoxLocation = animPos.GetBox2DVec();
	float BX = BoxLocation.x * 1000.0f;
	float BY = BoxLocation.y * 1000.0f;

	TestAnimationPosText->SetText(TT(" X : %d  ,  Y : %d  "), (int)animPos.X(), (int)animPos.Y());
	//MousePosText->SetText(TT("Box2D X : %d , Box2D Y : %d"), (int)BX, (int)BY);
	MousePosText->SetText(TT("Box2D X : %d , Box2D Y : %d"), (int)MousePos.X(), (int)MousePos.Y());
}
void ExistObject::Right()
{
	TestAnimation->AddComponentLocation(1.0f, 0.0f);
	TestAnimation->SetCurrentState(AnimationState::Right);
}
void ExistObject::Left()
{
	TestAnimation->AddComponentLocation(-1.0f, 0.0f);
	TestAnimation->SetCurrentState(AnimationState::Left);
}
void ExistObject::Up()
{
	TestAnimation->AddComponentLocation(0.0f, -1.0f);
	TestAnimation->SetCurrentState(AnimationState::Up);
}
void ExistObject::Down()
{
	TestAnimation->AddComponentLocation(0.0f, 1.0f);
	TestAnimation->SetCurrentState(AnimationState::Down);
}





