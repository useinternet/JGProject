#include"LoadingWidget.h"
#include"EngineFrameWork/2D/Widget/ImageBox.h"
#include"EngineFrameWork/Components/LoadingComponent.h"
#include"EngineFrameWork/World/World.h"
#include"EngineFrameWork/World/WorldManager.h"
#include"EngineFrameWork/2D/Widget/Progressbar.h"
#include"EngineStatics/JGLog.h"
LoadingWidget::LoadingWidget()
{
	RegisterObjectID(typeid(this));


	LoadingBackGroundImage = RegisterComponentInObject<ImageBox>(TT("LoadingBackGroundImage"));
	LoadingBackGroundImage->CreateImage(
		TT("../Contents/LoadingWidget/LoadingBackGround.jpg"),EPivot::TopLeft);

	LoadingSupporter = RegisterComponentInObject<LoadingComponent>(TT("LoadingSupporter"));
	


	SampleBar = RegisterComponentInObject<Progressbar>(TT("SampleBar"));
	SampleBar->CreateProgressBar(
		TT("../Contents/CommonUI/ProgressBar_Loading_Blue.png"), TT("../Contents/CommonUI/ProgressBar_Loading_Back.png"),EPivot::MiddleMiddle);
	SampleBar->SetBindingData(0.0f);
	SampleBar->SetComponentScale(1.0f, 2.0f);
	SampleBar->SetComponentLocation(960.0f, 940.0f);
	JGLog::Write(ELogLevel::Default, TT("StartMain World Start!"));

	ActiveObject();
}
LoadingWidget::~LoadingWidget()
{

}
void LoadingWidget::BeginObject(World * world)
{
	UserWidget::BeginObject(world);
	JGLog::Write(ELogLevel::Default, TT("Loading World Start!"));
	LoadingSupporter->OpenWorld(TT("DemoWorld"));
}
void LoadingWidget::Tick(const float DeltaTime)
{
	UserWidget::Tick(DeltaTime);
	float progress = LoadingSupporter->GetCurrentProgress();
	SampleBar->SetBindingData(progress);
	static float acctime = 0.0f;
	acctime += DeltaTime;
	if (acctime >= 1.0f)
	{
		acctime = 0.0f;
		JGLog::Write(ELogLevel::Default, TT("Loading : %d %%"), (int)(progress * 100));
	}
	if (progress == 1.0f && !ChangeWorld)
	{
		JGLog::Write(ELogLevel::Default, TT("Loading Compelete"));
		ChangeWorld = true;
		GetWorld()->GetManager()->SelectWorld(TT("DemoWorld"),false);
		JGLog::Write(ELogLevel::Default, TT("Demo World Start!"));
	}
}
