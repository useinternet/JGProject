#include"WorldManager.h"
#include"World.h"
#include"../../PhysicsSystem/JGBox2D/JGPhysicsSystem.h"
#include"../../PhysicsSystem/JGBox2D/JGDynamics/JGPhysicsWorld.h"
#include"../../EngineStatics/JMath/JGVector2D.h"
#include"../Object/ObjectSpawner.h"
#include"../../EngineStatics/JGLog.h"
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
		auto iter = m_mWorlds.find(m_PrevWorld);
		if (iter != m_mWorlds.end())
		{
			m_mWorlds[m_PrevWorld]->Clear();
		}
		m_PrevWorld = m_CurrentWorld;
	}
	//
	if (m_CurrentWorld != TT("None"))
	{
		m_mWorlds[m_CurrentWorld]->Tick(DeltaTime);
	}
}
void WorldManager::SelectWorld(const std::wstring& worldName,const bool spawn)
{
	m_CurrentWorld = worldName;
	if (spawn)
	{
		m_mWorldSpawners[m_CurrentWorld]->Spawn(m_mWorlds[m_CurrentWorld].get());
	}
}
void WorldManager::OpenWorld(const std::wstring& worldName)
{
	auto result = m_mWorlds.find(worldName);
	if (result == m_mWorlds.end())
	{
		JGLog::Write(ELogLevel::Warning, TT("This world name does not exist in the world manager."));
		return;
	}
	m_mWorlds[worldName]->Clear();
	m_mWorldSpawners[worldName]->Spawn(m_mWorlds[worldName].get());
}
void WorldManager::AddWorld(const wstring& worldName, ObjectSpawner* spawner)
{
	JGVector2D Gravity(0.0f, 980.0f);
	JGPhysicsWorld* pyWorld = m_pPhysicsSystem->AddPhysicsWorld(worldName, Gravity);
	unique_ptr<World> world = make_unique<World>(worldName, pyWorld,this);
	m_mWorldSpawners.insert(pair<const wstring, ObjectSpawner*>(worldName, spawner));
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