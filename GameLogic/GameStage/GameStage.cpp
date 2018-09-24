#include"GameStage.h"
#include"../GameLogic.h"
#include"EngineFrameWork/GameLoop.h"
#include"EngineFrameWork/World/WorldManager.h"
#include"EngineFrameWork/World/World.h"
#include"SampleWorldSpawner.h"

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
	m_WorldManager->AddWorld(TT("SampleWorld"));
	m_WorldManager->SelectWorld(TT("SampleWorld"));
	m_WorldManager->GetWorld(TT("SampleWorld"))->SpawnObject<SampleWorldSpawner>();



}
