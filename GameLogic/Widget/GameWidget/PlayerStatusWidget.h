#pragma once
#include"EngineFrameWork/Object/UserWidget.h"


class PlayerStatusWidget : public UserWidget
{
private:
	class Player* player;
	class Progressbar* HpBar;
	class Progressbar* StaminaBar;
public:
	PlayerStatusWidget();
	virtual ~PlayerStatusWidget();
	virtual void BeginObject(World* world);
	virtual void Tick(const float DeltaTime);
};