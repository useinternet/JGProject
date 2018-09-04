#pragma once
#include"../EngineStatics/Engine.h"

class RenderSystem;
class WorldManager;
class ENGINE_EXPORT GameLoop
{
private:
	std::unique_ptr<WorldManager> m_WorldManager;
	RenderSystem* m_pRenderSystem;


	
public:
	GameLoop();
	~GameLoop();
	void InitGameLoop(RenderSystem* renderSystem);

	void Tick(const float DeltaTime);
};