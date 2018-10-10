#include"GameModeBase.h"
#include"EngineFrameWork/World/World.h"
#include"Character/Player.h"
#include"Character/PlayerStatus.h"
#include"EngineStatics/JGLog.h"
GameModeBase::GameModeBase()
{
	RegisterObjectID(typeid(this));

}

GameModeBase::~GameModeBase()
{
}

void GameModeBase::SetPlayer(Player * player)
{
	JGLog::Write(ELogLevel::Progress, TT("mode connect"));
	GamePlayer = player;
}

Player* GameModeBase::GetPlayer()
{
	return GamePlayer;
}

bool GameModeBase::ConditionGameEnd()
{
	if (GamePlayer == nullptr)
	{
		return false;
	}
	if (GamePlayer->IsDie())
	{
		return true;
	}
	return false;
}

void GameModeBase::GameEndEvent(const float DeltaTime)
{

}
