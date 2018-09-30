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
	std::wstring name = TT("SampleWorld");
	m_WorldManager->AddWorld(name);
	m_WorldManager->SelectWorld(TT("SampleWorld"));
	m_WorldManager->GetWorld(TT("SampleWorld"))->SpawnObject<SampleWorldSpawner>();



}
