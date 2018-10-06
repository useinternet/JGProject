#include"LoadingWidget.h"
#include"EngineFrameWork/2D/Widget/ImageBox.h"
#include"EngineFrameWork/Components/LoadingComponent.h"
#include"EngineFrameWork/World/World.h"
#include"EngineFrameWork/World/WorldManager.h"
#include"EngineStatics/JGLog.h"
LoadingWidget::LoadingWidget()
{
	RegisterObjectID(typeid(this));


	LoadingBackGroundImage = RegisterComponentInObject<ImageBox>(TT("LoadingBackGroundImage"));
	LoadingBackGroundImage->CreateImage(
		TT("../Contents/LoadingWidget/LoadingBackGround.jpg"),EPivot::TopLeft);

	LoadingSupporter = RegisterComponentInObject<LoadingComponent>(TT("LoadingSupporter"));
	


	ActiveObject();
}
LoadingWidget::~LoadingWidget()
{

}
void LoadingWidget::BeginObject(World * world)
{
	JGLog::Write(ELogLevel::Error, TT(" Object Begin error"));
	UserWidget::BeginObject(world);
	LoadingSupporter->OpenWorld(TT("DemoWorld"));

}
void LoadingWidget::Tick(const float DeltaTime)
{
	UserWidget::Tick(DeltaTime);
	float progress = LoadingSupporter->GetCurrentProgress();
	static float acctime = 0.0f;
	acctime += DeltaTime;
	if (acctime >= 2.0f)
	{
		acctime = 0.0f;
		JGLog::Write(ELogLevel::Default, TT("Progress : %d %%"), (int)(progress * 100));
	}
	if (progress >= 0.5f)
	{
		int num = 0;
	}
	if (progress == 1.0f && !ChangeWorld)
	{
		ChangeWorld = true;
		GetWorld()->GetManager()->SelectWorld(TT("DemoWorld"),false);
	}
}
