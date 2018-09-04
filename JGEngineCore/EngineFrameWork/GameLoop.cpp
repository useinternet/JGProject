#include"GameLoop.h"
#include"World/WorldManager.h"
#include"World/World.h"
#include"../RenderSystem/RenderSystem.h"
using namespace std;


GameLoop::GameLoop()
{
}
GameLoop::~GameLoop()
{

}
void GameLoop::InitGameLoop(RenderSystem* renderSystem)
{
	m_pRenderSystem = renderSystem;


	// 임시 
	m_WorldManager = make_unique<WorldManager>();

	// 임시
	m_WorldManager->AddWorld(TT("SampleWorld"));
	m_WorldManager->SelectWorld(TT("SampleWorld"));
}

void GameLoop::Tick(const float DeltaTime)
{
	if (m_WorldManager)
	{
		if (m_WorldManager->GetIsChangeWorld())
		{
			m_pRenderSystem->ReceiveObjectArray(
				m_WorldManager->GetCurrentWorld()->GetObjectArray()
			);
		}
		m_WorldManager->Tick(DeltaTime);
	}

}