#include"EngineBridge.h"
#include"JGMainCore.h"


void InitEngine(HWND hWnd)
{
	g_Engine = new JGMainCore;
	g_Engine->InitCore(hWnd);
}

void Run()
{
	g_Engine->Run_Edt();
}

void End()
{
	g_Engine->Destroy();
	delete g_Engine;
	g_Engine = nullptr;
}

void EG2ED_SendCommand(const char* Command, void* Data, unsigned int Size)
{

}
void ED2EG_SendCommand(const char* Command, void* Data, unsigned int Size)
{

}