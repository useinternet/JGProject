#include "pch.h"
#include "Game.h"
#include "DataType/Plugin.h"
#include "Object/GameObject.h"
using namespace std;

namespace GFW
{
	Game::Game(const GlobalLinkStream& stream)
		: EngineCore(stream)
	{
		GFW_LOG_INFO("GameFrameWork Init Compelete");
	}

	void Game::Load()
	{
		Plugin p;
		p.Load("JGGame.dll");
		if (p.IsValid())
		{
			pCreateGameObjectFunc func;
			func = (pCreateGameObjectFunc)p.GetProcAddress("CreateTestObject");
			if (func)
			{
				auto obj = func();
				std::shared_ptr<GameObject> ptr(obj);


				ptr->Begin();
				ptr->Tick();
				ptr->End();
			}
		}
		else
		{
			GFW_LOG_ERROR("Failed Load Plugin");
		}

	}

	void Game::Update()
	{
		ENGINE_PERFORMANCE_TIMER("Application", "Game");
	}
	void Game::OnEvent(Event& e)
	{

	}
}