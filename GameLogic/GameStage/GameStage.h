#pragma once
#include"EngineStatics/Engine.h"
#include"EngineFrameWork/Object/ObjectSpawner.h"
class GameLoop;
class WorldManager;
class GameStage
{
private:
	std::vector<std::shared_ptr<ObjectSpawner>> m_vSpawners;
	GameLoop* m_Loop = nullptr;
	WorldManager* m_WorldManager = nullptr;

public:
	GameStage();
	~GameStage();
	void InitStage(GameLoop* Loop);
	void ManagementWorld();
	template<typename SpawnerType>
	SpawnerType* CreateObjectSpawner();
};

template<typename SpawnerType>
inline SpawnerType* GameStage::CreateObjectSpawner()
{
	SpawnerType* result = nullptr;
	std::unique_ptr<SpawnerType> spawner = std::make_unique<SpawnerType>();
	result = spawner.get();
	m_vSpawners.push_back(std::move(spawner));
	return result;
}
