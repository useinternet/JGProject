#include"ExistObject.h"
#include"../Components/StaticMesh2DComponent.h"
#include"../../EngineStatics/JGConstructHelper.h"
#include"../../RenderSystem/JGMaterial/JG2DMesh.h"
ExistObject::ExistObject()
{
	RegisterObjectID(typeid(this));

	// 등록
	Breath = RegisterComponentInObject<StaticMesh2DComponent>(TT("Breath"));
	

	const JGConstructHelper::StaticMesh2D BreathMesh(GetDevice(),
		GetBufferManager(), Breath->GetComponentName(),
		EPivot::MiddleMiddle, TT("../ManagementFiles/Resource/Breath.png"));
	if (BreathMesh.Success)
	{
		Breath->SetStaticMesh2DComponent(BreathMesh.Object);
	}
	// 등록
	Test = RegisterComponentInObject<StaticMesh2DComponent>(TT("Test"));

	const JGConstructHelper::StaticMesh2D TestTexture(GetDevice(),
		GetBufferManager(), Breath->GetComponentName(), EPivot::MiddleMiddle,
		TT("../ManagementFiles/Resource/Sample.jpg"));
	if (TestTexture.Success)
	{
		Test->SetStaticMesh2DComponent(TestTexture.Object);
	}
	Breath->AddChild(Test);
}
ExistObject::~ExistObject()
{


}

void ExistObject::BeginObject()
{
	Object::BeginObject();
	Breath->SetComponentLocation(800.0f, 500.0f);
	Test->SetComponentLocation(100.0f, 100.0f);
}

void ExistObject::Tick(const float DeltaTime)
{
	Object::Tick(DeltaTime);
	Breath->AddComponentAngle(DeltaTime*3);
	Test->AddComponentAngle(DeltaTime * 2);

}
