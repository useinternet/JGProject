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
			pCreateGameObjectFunc create_func;
			pDeleteGameObjectFunc delete_func;
			create_func = (pCreateGameObjectFunc)p.GetProcAddress("CreateTestObject");
			delete_func = (pDeleteGameObjectFunc)p.GetProcAddress("DeleteTestObject");
			if (create_func)
			{
				auto obj = create_func();


				obj->Begin();
				obj->Tick();
				obj->End();

				delete_func(obj);
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