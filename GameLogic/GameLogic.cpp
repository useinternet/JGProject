#include"GameLogic.h"


void  GameLogic::Init(HINSTANCE Instance, HWND hWnd)
{
	Engine = new JGEngineMain;
	Engine->Init(Instance, hWnd);
	Stage = new GameStage;
	Stage->InitStage(Engine->GetGameLoop());
	Stage->ManagementWorld();
}
void  GameLogic::Run()
{
	Engine->Run();
}

void GameLogic::DoEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	Engine->DoEvent(message, wParam, lParam);
}
void GameLogic::Destroy()
{
	delete Stage;
	Stage = nullptr;
	Engine->Destroy();
	delete Engine;
	Engine = nullptr;
}