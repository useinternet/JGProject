#include"StartMainWidget.h"
#include"EngineFrameWork/2D/Widget/ImageBox.h"
#include"EngineFrameWork/2D/Widget/Button.h"
#include"EngineFrameWork/2D/Widget/Text.h"
#include"EngineFrameWork/Components/TextComponent.h"
#include"EngineFrameWork/World/World.h"
#include"EngineFrameWork/World/WorldManager.h"
#include"EngineStatics/JGLog.h"


// 임시
#include"EngineFrameWork/2D/Widget/Progressbar.h"
using namespace std;

static const wstring ButtonDefaultImagePath = TT("../Contents/MainStartWidget/buttonLong_beige.png");
static const wstring ButtonPressImagePath = TT("../Contents/MainStartWidget/buttonLong_beige_pressed.png");
static const string  Font_fnt_Path   = "../Contents/Font/Godic.fnt";
static const wstring Font_Image_Path = TT("../Contents/Font/Godic_0.png");
static const JGVector2D StartButtonLocation(960.0f, 640.0f);
static const JGVector2D ConfigButtonLocation(960.0f, 790.0f);
static const JGVector2D ExpButtonLocation(960.0f, 940.0f);
static const JGVector2D GameTitleLocation(600.0f, 150.0f);
static const JGVector2D GameStartLocation(825.0f, 555.0f);
static const JGVector2D GameConfigLocation(825.0f, 705.0f);
static const JGVector2D GameExpLocation(825.0f, 855.0f);


StartMainWidget::StartMainWidget()
{
	RegisterObjectID(typeid(this));

	MainBackGround = RegisterComponentInObject<ImageBox>(TT("MainBackGround"));
	MainBackGround->CreateImage(TT("../Contents/MainStartWidget/MainBackGround.jpg"), EPivot::TopLeft);
	MainBackGround->ReSizeImage(1920.0f, 1080.0f);

	StartButton = RegisterComponentInObject<Button>(TT("StartButton"));
	StartButton->CreateButton(EPivot::BottomMiddle,
		ButtonDefaultImagePath, ButtonPressImagePath, ButtonDefaultImagePath);
	ConfigButton = RegisterComponentInObject<Button>(TT("ConfigButton"));
	ConfigButton->CreateButton(EPivot::BottomMiddle,
		ButtonDefaultImagePath, ButtonPressImagePath, ButtonDefaultImagePath);
	ExpButton = RegisterComponentInObject<Button>(TT("ExpButton"));
	ExpButton->CreateButton(EPivot::BottomMiddle,
		ButtonDefaultImagePath, ButtonPressImagePath, ButtonDefaultImagePath);

	GameTitle = RegisterComponentInObject<Text>(TT("GameTitle"));
	GameTitle->SetFont(Font_fnt_Path,Font_Image_Path);
	GameTitle->Get()->SetText(TT("정희 엔진 데모 게임"));
	GameStart = RegisterComponentInObject<Text>(TT("GameStart"));
	GameStart->SetFont(Font_fnt_Path, Font_Image_Path);
	GameStart->Get()->SetText(TT("게임 시작"));
	GameExp = RegisterComponentInObject<Text>(TT("GameExp"));
	GameExp->SetFont(Font_fnt_Path, Font_Image_Path);
	GameExp->Get()->SetText(TT("게임 설명"));
	GameConfig = RegisterComponentInObject<Text>(TT("GameConfig"));
	GameConfig->SetFont(Font_fnt_Path, Font_Image_Path);
	GameConfig->Get()->SetText(TT("환경 설정"));



	GameTitle->Get()->SetTextSize(50.0f);
	GameStart->Get()->SetTextSize(40.0f);
	GameConfig->Get()->SetTextSize(40.0f);
	GameExp->Get()->SetTextSize(40.0f);


	GameTitle->Get()->SetComponentLocation(GameTitleLocation);
	GameStart->Get()->SetComponentLocation(GameStartLocation);
	GameConfig->Get()->SetComponentLocation(GameConfigLocation);
	GameExp->Get()->SetComponentLocation(GameExpLocation);



	StartButton->SetComponentScale(2.0f);
	ConfigButton->SetComponentScale(2.0f);
	ExpButton->SetComponentScale(2.0f);


	StartButton->SetComponentLocation(StartButtonLocation);
	ConfigButton->SetComponentLocation(ConfigButtonLocation);
	ExpButton->SetComponentLocation(ExpButtonLocation);


	


	ActiveObject();

}

StartMainWidget::~StartMainWidget()
{

}
void StartMainWidget::BeginObject(World* world)
{
	UserWidget::BeginObject(world);
}
void StartMainWidget::Tick(const float DeltaTime)
{
	UserWidget::Tick(DeltaTime);
	ChangeTextLocationWithButtonClick(StartButton, GameStart, GameStartLocation);
	ChangeTextLocationWithButtonClick(ConfigButton, GameConfig, GameConfigLocation);
	ChangeTextLocationWithButtonClick(ExpButton, GameExp, GameExpLocation);

	if (StartButton->IsButtonClick())
	{
		GetWorld()->GetManager()->SelectWorld(TT("Loading"));
	}
}
void StartMainWidget::ChangeTextLocationWithButtonClick(Button* button, Text* text, const JGVector2D& TextLocation)
{
	if (button->IsMousePressedInButton())
	{
		JGVector2D TransLocation = TextLocation;
		TransLocation.SetY(TransLocation.Y() + 10.0f);
		text->Get()->SetComponentLocation(TransLocation);
	}
	else
	{
		text->Get()->SetComponentLocation(TextLocation);
	}
}
