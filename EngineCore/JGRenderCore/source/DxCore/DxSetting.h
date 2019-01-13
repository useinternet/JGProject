#pragma once
#include"DxCommon/DxCommon.h"

typedef struct RCORE_EXPORT DxSetting
{
	int Width  = 1920;
	int Height = 1080;
	float NearZ = 1.0f;
	float FarZ = 1000.0f;
	HWND hWnd;
	bool FullScreen = false;

}DxSetting;