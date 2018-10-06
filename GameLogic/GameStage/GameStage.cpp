#include"GameStage.h"
#include"../GameLogic.h"
#include"EngineFrameWork/GameLoop.h"
#include"EngineFrameWork/World/WorldManager.h"
#include"EngineFrameWork/World/World.h"
#include"WorldSpawner/StartMainMenu/StartMainMenuSpawner.h"
#include"WorldSpawner/LoadingWorld/LoadingSpawner.h"
#include"WorldSpawner/DemoWorld/DemoWorldSpawner.h"
GameStage::GameStage()
{

}
GameStage::~GameStage()
{

}
void GameStage::InitStage(GameLoop* Loop)
{
	m_Loop = Loop;
	m_WorldManager = Loop->GetWorldManager();
}

void GameStage::ManagementWorld()
{
	m_WorldManager->AddWorld(TT("StartMainMenu"),
		CreateObjectSpawner<StartMainMenuSpawner>());
	m_WorldManager->AddWorld(TT("Loading"),
		CreateObjectSpawner<LoadingSpawner>());
	m_WorldManager->AddWorld(TT("DemoWorld"),
		CreateObjectSpawner<DemoWorldSpawner>());
	m_WorldManager->SelectWorld(TT("StartMainMenu"));
}
