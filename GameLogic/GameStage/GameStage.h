#pragma once
#include"EngineStatics/Engine.h"
class GameLoop;
class WorldManager;

class GameStage
{
private:
	GameLoop * m_Loop = nullptr;
	WorldManager* m_WorldManager = nullptr;

public:
	GameStage();
	~GameStage();
	void InitStage(GameLoop* Loop);
	void ManagementWorld();
};