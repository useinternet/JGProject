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
using namespace std;
ExistObject::ExistObject()
{
	RegisterObjectID(typeid(this));

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



	// 입력 실험
	Input = RegisterComponentInObject<InputComponent>(TT("SampleInput"));
	sound = RegisterComponentInObject<SoundComponent>(TT("SoundSampleComponent"));
	sound->CreateSound("../ManagementFiles/Resource/Music/Always-_2_.wav", ESoundMode::Stream2D);

	image = RegisterComponentInObject<ImageBox>(TT("SampleImageBox"));
	image->CreateImage(TT("../ManagementFiles/Resource/Breath.png"), EPivot::TopLeft);
}
ExistObject::~ExistObject()
{


}

void ExistObject::BeginObject()
{
	Object::BeginObject();
	Frame->SetComponentLocation(300.0f, 500.0f);
	Input->BindKeyCommand(TT("Right"), EKeyState::Down,
		bind(&ExistObject::Right, this));
	Input->BindKeyCommand(TT("Left"), EKeyState::Down,
		bind(&ExistObject::Left, this));
	Input->BindKeyCommand(TT("Up"), EKeyState::Down,
		bind(&ExistObject::Up, this));
	Input->BindKeyCommand(TT("Down"), EKeyState::Down,
		bind(&ExistObject::Down, this));
	sound->Play();
}

void ExistObject::Tick(const float DeltaTime)
{
	Object::Tick(DeltaTime);
	// 임시 프레임 알아보기
	float FPS = 1.0f / DeltaTime;
	Frame->SetText(TT("FPS : %d"), (int)FPS);

	static float acctime = 0;
	acctime += DeltaTime;
	if (acctime >= 5.0f)
	{
		sound->Pause();
	}
	if (acctime >= 10.0f)
	{
		sound->Play();
	}
	image->AddBlend(DeltaTime * (-0.3f));
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





