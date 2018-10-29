#pragma once
#include<Windows.h>

namespace JGRC
{
	class JGInitConfig
	{
	public:
		HWND hWnd;
		bool bFullScreen  = false;
		int  ScreenWidth  = 1920;
		int  ScreenHeight = 1080;
		float Fov   = 3.141592f / 4.0f;
		float FarZ  = 1000.0f;
		float NearZ = 0.1f;
	};
}