#include"InterpBridge.h"




void InitEngine(HINSTANCE Instance, HWND hWnd, int ScreenWidth, int ScreenHeight)
{
	g_sample = new SampleClass;
	g_sample->Init(Instance,hWnd);
}
void Start()
{
	g_sample->Run();
}

void End()
{
	g_sample->Destroy();
	delete g_sample;
	g_sample = nullptr;
}

void EG2ED_SendCommand(const char* Command, void* Data, unsigned int Size)
{

}
void ED2EG_SendCommand(const char* Command, void* Data, unsigned int Size)
{

}