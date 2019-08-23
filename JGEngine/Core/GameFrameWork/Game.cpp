#include "pch.h"
#include "Game.h"

using namespace std;

namespace GFW
{
	Game::Game(const GlobalLinkStream& stream)
		: EngineCore(stream)
	{
		GFW_LOG_INFO("GameFrameWork Init Compelete");
	}

	void Game::Update()
	{

	}
	void Game::OnEvent(Event& e)
	{

	}
}