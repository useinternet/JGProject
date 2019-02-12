#pragma once
#include"DxCommon/DxCommon.h"

typedef struct RCORE_EXPORT SceneConfig
{
	int   Width  = 1920;
	int   Height = 1080;
	float NearZ  = 1.0f;
	float FarZ   = 100000.0f;
	bool  FullScreen = false;
	HWND  hWnd;

	int  ShadowWidth  = 2048;
	int  ShadowHeight = 2048;
}SceneConfig;