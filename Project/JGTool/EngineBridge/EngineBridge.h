#pragma once
#include<Windows.h>
#ifdef ENGINEBRIDGE_EXPORTS
#define BRIDGE_EXPORT __declspec(dllexport)
#else
#define BRIDGE_EXPORT __declspec(dllimport)
#endif
#ifdef _DEBUG
#pragma comment(lib,"JGMainCore_d.lib")
#else
#pragma comment(lib,"JGMainCore.lib")
#endif



// x64 x32 개발 모드는 여기로
// x64 x32 디버그/릴리즈(실행파일모드는) 윈도우실행 디버그 모드는 파일로..
// x64 x32 디버그/릴리즈(실행파일모드는) 윈도우실행 디버그 모드는 파일로..

extern "C"
{
	BRIDGE_EXPORT class JGMainCore* g_Engine = nullptr;
	BRIDGE_EXPORT void InitEngine(HWND hWnd);
	
	BRIDGE_EXPORT void Run();

	BRIDGE_EXPORT void End();

	BRIDGE_EXPORT void EG2ED_SendCommand(const char* Command, void* Data, unsigned int Size);
	BRIDGE_EXPORT void ED2EG_SendCommand(const char* Command, void* Data, unsigned int Size);
}