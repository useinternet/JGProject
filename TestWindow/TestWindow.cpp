// TestWindow.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "TestWindow.h"

#define MAX_LOADSTRING 100
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include"TestWin.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	TestWindow* win = new TestWindow(hInstance);
	win->Initialize();
	win->Run();
	delete win;
    return 0;
}