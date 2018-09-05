#pragma once
#include<Windows.h>
#include"../GameLogic/Sample.h"


#pragma comment(lib,"GameLogic.lib")
#define BridgeExport __declspec(dllexport)


// x64 x32 개발 모드는 여기로
// x64 x32 디버그/릴리즈(실행파일모드는) 윈도우실행 디버그 모드는 파일로..
// x64 x32 디버그/릴리즈(실행파일모드는) 윈도우실행 디버그 모드는 파일로..

extern "C"
{
	BridgeExport SampleClass* g_sample = nullptr;
	BridgeExport void InitEngine(HINSTANCE Instance, HWND hWnd, int ScreenWidth, int ScreenHeight);

	BridgeExport void Start();

	BridgeExport void End();

	BridgeExport void EG2ED_SendCommand(const char* Command, void* Data, unsigned int Size);
	BridgeExport void ED2EG_SendCommand(const char* Command, void* Data, unsigned int Size);
}