#pragma once
#include"EngineFrameWork/Object/GameMode.h"

class Player;
class GameModeBase : public GameMode
{
private:
	Player* GamePlayer;
public:
	GameModeBase();
	virtual ~GameModeBase();
	void SetPlayer(Player* player);
	Player* GetPlayer();
	virtual bool ConditionGameEnd() override;
	virtual void GameEndEvent(const float DeltaTime) override;
	
};