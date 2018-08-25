#include"Sample.h"


void  SampleClass::Init(HWND hWnd)
{
	System = new JGEngineMain;
	System->Init(hWnd);
}
void  SampleClass::Run()
{
	System->Run();
}

void SampleClass::DoEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	System->DoEvent(message, wParam, lParam);
}
void SampleClass::Destroy()
{
	System->Destroy();
	delete System;
	System = nullptr;
}