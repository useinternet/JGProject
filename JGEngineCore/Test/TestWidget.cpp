#include"TestWidget.h"
#include"../EngineFrameWork/2D/Widget/Button.h"
#include"../RenderSystem/JGMaterial/JG2DMesh.h"
#include"../EngineFrameWork/Components/TextComponent.h"
#include"../EngineStatics/JGConstructHelper.h"
#include"../EngineFrameWork/2D/Widget/Text.h"
#include"../EngineFrameWork/Components/SoundComponent.h"
TestWidget::TestWidget()
{
	RegisterObjectID(typeid(this));

	TestButton = RegisterComponentInObject<Button>(TT("TestButton"));
	TestButton->CreateButton(EPivot::TopLeft,
		TT("../ManagementFiles/Resource/ButtonSampleOrd.png"),
		TT("../ManagementFiles/Resource/ButtonSampleDown.png"),
		TT("../ManagementFiles/Resource/ButtonSamplePut.png"));
	TestButton->SetComponentLocation(1700.0f, 0.0f);


	ButtonPos = RegisterComponentInObject<TextComponent>(TT("ButtonPosText"));
	static JGConstructHelper::TextFont ButtonPosMesh(
		GetDevice(), "../ManagementFiles/Resource/Font/Godic.fnt",
		TT("../ManagementFiles/Resource/Font/Godic_0.png"));
	if (ButtonPosMesh.Success)
	{
		ButtonPos->SetConstructObject(ButtonPosMesh.Object);
	}
	ButtonPos->SetComponentLocation(0.0f, 150.0f);


	MusicText = RegisterComponentInObject<Text>(TT("MusicText"));
	MusicText->SetFont(
		"../ManagementFiles/Resource/Font/Godic.fnt", TT("../ManagementFiles/Resource/Font/Godic_0.png"));
	MusicText->Get()->SetTextSize(20.0f);
	MusicText->Get()->SetText(TT("Àç»ý / ¸ØÃã"));
	TestButton->AddChild(MusicText);
	MusicText->SetComponentLocation(5.0f, 90.0f);


	sound = RegisterComponentInObject<SoundComponent>(TT("SoundSampleComponent"));
	sound->CreateSound("../ManagementFiles/Resource/Music/Always-_2_.wav", ESoundMode::Stream2D);
	//sound->Play();
}

TestWidget::~TestWidget()
{

}

void TestWidget::Tick(const float DeltaTime)
{
	UserWidget::Tick(DeltaTime);
	ButtonPos->SetText(TT("ButtonX : %d MouseY : %d "),
		(int)TestButton->GetComponentWorldLocation().X(),
		(int)TestButton->GetComponentWorldLocation().Y());

	static bool test = false;

	if (TestButton->IsMouseClickInButton())
	{
		if (!sound->IsPlaying() && !test)
		{
			test = true;
			sound->Play();
		}
		else if (sound->IsPlaying() && !test)
		{
			test = true;
			sound->Pause();
		}
	}
	else
	{
		test = false;
	}


}
