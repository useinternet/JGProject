#pragma once


#include <memory>
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <ostream>
#include <crtdbg.h>
#include "JE.h"
#include "Application.h"
#ifdef USE_JGENGINE_CONSOLE
int main()
{

	Application* app = new Application(L"JGEngine", EApplicationMode::Editor);

	if (app->Init())
	{
		app->Run();
	}
	delete app;
	app = nullptr;
	return 0;
}
#else
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Application* app = new Application(L"JGEngine", EApplicationMode::Editor);

	if (app->Init())
	{
		app->Run();
	}

	delete app;
	app = nullptr;
	return 0;
}
#endif



