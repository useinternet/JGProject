#include"GameLoop.h"
#include"World/WorldManager.h"
#include"World/World.h"
#include"../RenderSystem/RenderSystem.h"
#include"../PhysicsSystem/JGBox2D/JGPhysicsSystem.h"
#include"../EngineStatics/JMath/JGVector2D.h"
using namespace std;


GameLoop::GameLoop()
{

}
GameLoop::~GameLoop()
{

}
void GameLoop::InitGameLoop(RenderSystem* renderSystem, JGPhysicsSystem* physicsSystem)
{
	m_pRenderSystem = renderSystem;
	m_pPhysicsSystem = physicsSystem;

	// 임시 
	m_WorldManager = make_unique<WorldManager>();
	m_WorldManager->InitWorldManager(physicsSystem);
	// 임시
	m_WorldManager->AddWorld(TT("SampleWorld"));
	m_WorldManager->SelectWorld(TT("SampleWorld"));
	JGVector2D vec(0.0f, 9.8f);
	m_pPhysicsSystem->AddPhysicsWorld(TT("SampleWorld"), vec);
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
			m_pPhysicsSystem->SetCurrentPhysicsWorld(
				m_WorldManager->GetCurrentWorldName());
			m_pPhysicsSystem->DebugDrawSetViewMatrix(
				m_WorldManager->GetCurrentWorld()->GetViewMatrix());
		}
		m_WorldManager->Tick(DeltaTime);
	}
}

WorldManager* GameLoop::GetWorldManager()
{
	return m_WorldManager.get();
}
