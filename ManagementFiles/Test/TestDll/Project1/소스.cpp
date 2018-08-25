#include<iostream>
#include<Windows.h>
#include"../../Dll1/Dll1/Dll1.h"
#pragma comment(lib,"../../Dll1/Debug/Dll1.lib")



typedef void(*TestFunc)();

int main()
{
	HINSTANCE hInst;
	TestFunc func;
	while (true)
	{
		hInst = LoadLibrary("../../Dll1/Debug/Dll1.dll");
		if (hInst == nullptr)
		{
			continue;
		}
		func = (TestFunc)GetProcAddress(hInst, "HelloWolrd");
		if (func == nullptr)
		{
			continue;
		}
		else
		{
			func();
		}
	}

	FreeLibrary(hInst);
	return 0;
}