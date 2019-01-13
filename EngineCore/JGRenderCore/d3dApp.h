//***************************************************************************************
// d3dApp.h by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

#pragma once
#include"DxCommon/DxCommon.h"



class RCORE_EXPORT D3DApp
{
public:
    D3DApp(HINSTANCE hInstance);
    D3DApp(const D3DApp& rhs) = delete;
    D3DApp& operator=(const D3DApp& rhs) = delete;
	virtual ~D3DApp() {}

public:
	static D3DApp* mApp;
	static D3DApp* GetApp();
	HINSTANCE AppInst()const;
	HWND      MainWnd()const;


	int Run();
 
    virtual bool Initialize();
    virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	virtual void OnResize(int width, int height); 
	virtual void Update(const GameTimer& gt)=0;
    virtual void Draw(const GameTimer& gt)=0;

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y){ }
	virtual void OnMouseUp(WPARAM btnState, int x, int y)  { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y){ }

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

	GameTimer mTimer;
	int mClientWidth = 800;
	int mClientHeight = 600;
	std::wstring mMainWndCaption = L"TestWindow";
};

