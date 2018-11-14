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

#ifdef _DEBUG
#pragma comment(lib,"JGCommon/JGCommon_d.lib")
#else
#pragma comment(lib,"JGCommon/JGCommon.lib")
#endif
#ifdef _DEBUG
#pragma comment(lib,"JGRenderCore_d.lib")
#else
#pragma comment(lib,"JGRenderCore.lib")
#endif

extern "C"
{
	BRIDGE_EXPORT class JGMainCore* g_Engine = nullptr;
	BRIDGE_EXPORT void InitEngine(HWND hWnd);
	
	BRIDGE_EXPORT void Run();

	BRIDGE_EXPORT void End();

	BRIDGE_EXPORT void EG2ED_SendCommand(const unsigned int Command, void* Data, unsigned int Size);
	BRIDGE_EXPORT void ED2EG_SendCommand(const unsigned int Command, void* Data, unsigned int Size);
}