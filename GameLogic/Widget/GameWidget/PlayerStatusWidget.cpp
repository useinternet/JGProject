#include"PlayerStatusWidget.h"
#include"GameMode/GameModeBase.h"
#include"Character/Player.h"
#include"Character/PlayerStatus.h"
#include"EngineFrameWork/World/World.h"
#include"EngineFrameWork/2D/Widget/Progressbar.h"
PlayerStatusWidget::PlayerStatusWidget()
{
	RegisterObjectID(typeid(this));

	HpBar = RegisterComponentInObject<Progressbar>(TT("HpBar"));
	StaminaBar = RegisterComponentInObject<Progressbar>(TT("StaminaBar"));

	HpBar->CreateProgressBar(
		TT("../Contents/CommonUI/ProgressBar_Status_Red.png"),
		TT("../Contents/CommonUI/ProgressBar_Status_Back.png"),EPivot::TopLeft);
	HpBar->SetComponentLocation(50.0f, 50.0f);
	StaminaBar->CreateProgressBar(
		TT("../Contents/CommonUI/ProgressBar_Status_Yellow.png"),
		TT("../Contents/CommonUI/ProgressBar_Status_Back.png"),EPivot::TopLeft);
	StaminaBar->SetComponentLocation(50.0f, 75.0f);

	ActiveObject();
}
PlayerStatusWidget::~PlayerStatusWidget()
{

}

void PlayerStatusWidget::BeginObject(World* world)
{
	UserWidget::BeginObject(world);
	
	GameModeBase* mode = dynamic_cast<GameModeBase*>(world->GetGameMode());
	if (mode)
	{
		player = mode->GetPlayer();
	}
}

void PlayerStatusWidget::Tick(const float DeltaTime)
{
	UserWidget::Tick(DeltaTime);
	HpBar->SetBindingData(player->GetStatus()->GetHp() / player->GetStatus()->GetMaxHp());
	StaminaBar->SetBindingData(player->GetStatus()->GetStamina() / player->GetStatus()->GetMaxStamina());
}
