//***************************************************************************************
// JGWindow.h by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

#pragma once
#include<Windows.h>
#include<string>
#include<crtdbg.h>
#include"GameTimer.h"
#include"DirectXToolKit/DirectXCore.h"

#ifdef _DEVELOP_DEBUG
#pragma comment(lib, "DirectX12_develop.lib")
#pragma comment(lib, "Common_develop.lib")
#pragma comment(lib, "imgui_develop.lib")
#pragma comment(lib, "GUIComponent_develop.lib")
#pragma comment(lib, "AssetManager_develop.lib")
#endif

#ifdef _DEVELOP_RELEASE
#pragma comment(lib, "DirectX12_developR.lib")
#pragma comment(lib, "Common_developR.lib")
#pragma comment(lib, "imgui.lib")
#pragma comment(lib, "GUIComponent_developR.lib")
#pragma comment(lib, "AssetManager_developR.lib")
#endif

#ifdef _RELEASE
#pragma comment(lib, "DirectX12.lib")
#pragma comment(lib, "Common.lib")
#pragma comment(lib, "imgui.lib")
#pragma comment(lib, "GUIComponent.lib")
#pragma comment(lib, "AssetManager.lib")
#endif
#include<imgui/imgui.h>
#include<imgui/imgui_impl_dx12.h>
#include<imgui/imgui_impl_win32.h>
#include<InformationViewer/Gui_Log.h>
#include<AssetManager.h>

class JGWindow
{
public:
    JGWindow(HINSTANCE hInstance);
    JGWindow(const JGWindow& rhs) = delete;
    JGWindow& operator=(const JGWindow& rhs) = delete;
	virtual ~JGWindow() {}

public:
	static JGWindow* mApp;
	static JGWindow* GetApp();
	HINSTANCE AppInst()const;
	HWND      MainWnd()const;


	int Run();
 
    virtual bool Initialize();
	virtual void Destroy();
    virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
protected:
	virtual void OnResize(int width, int height); 
	virtual void Update()=0;
    virtual void Draw()=0;

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y){ }
	virtual void OnMouseUp(WPARAM btnState, int x, int y)  { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y){ }
	virtual void KeyDown(WPARAM wparam) {}
protected:

	bool InitMainWindow();
	void CalculateFrameStats();
protected:
    HINSTANCE mhAppInst = nullptr; // application instance handle
    HWND      mhMainWnd = nullptr; // main window handle
	bool      mAppPaused = false;  // is the application paused?
	bool      mMinimized = false;  // is the application minimized?
	bool      mMaximized = false;  // is the application maximized?
	bool      mResizing = false;   // are the resize bars being dragged?
    bool      mFullscreenState = false;// fullscreen enabled
	bool      mInit = false;
	GameTimer mTimer;
	int mClientWidth = 800;
	int mClientHeight = 600;
	std::wstring mMainWndCaption = L"TestWindow";
};

