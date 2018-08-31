#include"WorldManager.h"
#include"World.h"
using namespace std;

WorldManager::WorldManager()
{

}
WorldManager::~WorldManager()
{

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
	std::unique_ptr<World> world = make_unique<World>(worldName);

	m_mWorlds.insert(pair <const wstring, shared_ptr<World>>(worldName, move(world)));
}
World* WorldManager::GetWorld(const std::wstring& WorldName)
{
	return m_mWorlds[WorldName].get();
}
World* WorldManager::GetCurrentWorld()
{
	return m_mWorlds[m_CurrentWorld].get();
}
bool WorldManager::GetIsChangeWorld()
{
	return m_bWorldChange;
}