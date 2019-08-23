#pragma once


#include <memory>
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <ostream>
#include <crtdbg.h>
#include "JE.h"
#include "Application.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	Application* app = new Application(L"JGEngine", EApplicationMode::Editor);

	if (app->Init())
	{
		app->Run();
	}

	delete app;
	app = nullptr;
	return 0;
}



