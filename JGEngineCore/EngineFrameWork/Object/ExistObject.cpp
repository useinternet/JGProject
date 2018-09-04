#include"ExistObject.h"
#include"../Components/StaticMesh2DComponent.h"
#include"../../EngineStatics/JGConstructHelper.h"
#include"../../RenderSystem/JGMaterial/JG2DMesh.h"
#include"../Components/TextComponent.h"

//이밋
#include"../2D/Text/JGFontLoader.h"
ExistObject::ExistObject()
{
	RegisterObjectID(typeid(this));

	// 등록
	Breath = RegisterComponentInObject<StaticMesh2DComponent>(TT("Breath"));
	

	const JGConstructHelper::StaticMesh2D BreathMesh(GetDevice(),
		GetBufferManager(), Breath->GetComponentName(),
		EPivot::TopLeft, TT("../ManagementFiles/Resource/Breath.png"));
	if (BreathMesh.Success)
	{
		Breath->SetConstructObject(BreathMesh.Object);
	}
	Breath->SetColorRize(0.0f, 0.0f, 1.0f);
	Frame = RegisterComponentInObject<TextComponent>(TT("Text"));
	const JGConstructHelper::TextFont FrameMesh(
		GetDevice(),  "../ManagementFiles/Resource/Font/Godic.fnt",
		TT("../ManagementFiles/Resource/Font/Godic_0.png"));
	if (FrameMesh.Success)
	{
		Frame->SetConstructObject(FrameMesh.Object);
	}
	Frame->SetTextColor(1.0f, 0.0f, 0.0f);
}
ExistObject::~ExistObject()
{


}

void ExistObject::BeginObject()
{
	Object::BeginObject();
	Frame->SetComponentLocation(300.0f, 500.0f);

}

void ExistObject::Tick(const float DeltaTime)
{
	// 임시 프레임 알아보기
	float FPS = 1.0f / DeltaTime;
	Frame->SetText(TT("FPS : %d"),(int)FPS);
	Object::Tick(DeltaTime);
	Breath->AddComponentAngle(DeltaTime*5.0f);
	//Test->AddComponentAngle(DeltaTime * 2);
}
