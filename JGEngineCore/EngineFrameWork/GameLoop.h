#pragma once
#include"../EngineStatics/Engine.h"

class RenderSystem;
class WorldManager;
class JGPhysicsSystem;
class ENGINE_EXPORT GameLoop
{
private:
	std::unique_ptr<WorldManager> m_WorldManager;
	RenderSystem* m_pRenderSystem;
	JGPhysicsSystem* m_pPhysicsSystem;
public:
	GameLoop();
	~GameLoop();
	void InitGameLoop(RenderSystem* renderSystem, JGPhysicsSystem* physicsSystem);
	void Tick(const float DeltaTime);
	WorldManager* GetWorldManager();
};