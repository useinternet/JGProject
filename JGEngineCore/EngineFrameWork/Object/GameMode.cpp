#include"GameMode.h"

GameMode::GameMode()
{
	RegisterObjectID(typeid(this));


}

GameMode::~GameMode()
{

}

void GameMode::Tick(const float DeltaTime)
{
	Object::Tick(DeltaTime);

	if (ConditionGameEnd())
	{
		m_bGameEnd = true;
	}
}

bool GameMode::ConditionGameEnd()
{
	return false;
}

void GameMode::GameEndEvent(const float DeltaTime)
{
}

bool GameMode::IsGameEnd()
{
	return m_bGameEnd;
}
