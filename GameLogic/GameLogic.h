#pragma once

#include"EngineStatics/Engine.h"
#include"GameStage/GameStage.h"
#include"EngineStatics/JGEngineMain.h"

#ifdef GAME_LOGIC
#pragma comment(lib,"JGEngineCore.lib")
#define GAME_LOGIC_EXPORT __declspec(dllexport)
#else
#define GAME_LOGIC_EXPORT __declspec(dllimport)
#endif



class GAME_LOGIC_EXPORT GameLogic
{
private:
	JGEngineMain* Engine;
	GameStage* Stage;
public:
	void Init(HINSTANCE Instance, HWND hWnd);
	void Run();
	void DoEvent(UINT message, WPARAM wParam, LPARAM lParam);
	void Destroy();
};