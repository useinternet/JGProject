#include"WorldManager.h"
#include"World.h"
#include"../../PhysicsSystem/JGBox2D/JGPhysicsSystem.h"
#include"../../PhysicsSystem/JGBox2D/JGDynamics/JGPhysicsWorld.h"
#include"../../EngineStatics/JMath/JGVector2D.h"
using namespace std;

WorldManager::WorldManager()
{

}
WorldManager::~WorldManager()
{

}

void WorldManager::InitWorldManager(JGPhysicsSystem* pySystem)
{
	m_pPhysicsSystem = pySystem;
}


void WorldManager::Tick(const float DeltaTime)
{
	m_bWorldChange = false;
	//
	if (m_CurrentWorld != m_PrevWorld)
	{
		m_bWorldChange = true;
		m_PrevWorld = m_CurrentWorld;
	}
	//
	if (m_CurrentWorld != TT("None"))
	{
		m_mWorlds[m_CurrentWorld]->Tick(DeltaTime);
	}
}
void WorldManager::SelectWorld(const std::wstring& worldName)
{
	m_CurrentWorld = worldName;
}
void WorldManager::AddWorld(const wstring& worldName)
{
	JGVector2D Gravity(0.0f, 980.0f);
	JGPhysicsWorld* pyWorld = m_pPhysicsSystem->AddPhysicsWorld(worldName, Gravity);
	unique_ptr<World> world = make_unique<World>(worldName, pyWorld);

	m_mWorlds.insert(pair <const wstring, shared_ptr<World>>(worldName, move(world)));
}
World* WorldManager::GetWorld(const wstring& WorldName)
{
	return m_mWorlds[WorldName].get();
}
World* WorldManager::GetCurrentWorld()
{
	return m_mWorlds[m_CurrentWorld].get();
}
const wstring& WorldManager::GetCurrentWorldName()
{
	return m_CurrentWorld;
}
bool WorldManager::GetIsChangeWorld()
{
	return m_bWorldChange;
}