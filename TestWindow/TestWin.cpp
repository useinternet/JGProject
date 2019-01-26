#include"stdafx.h"
#include"TestWin.h"
#include"DxCommon/GameTimer.h"
TestWindow::TestWindow(HINSTANCE hInst) : D3DApp(hInst)
{

}
TestWindow::~TestWindow()
{
	delete core;
}
bool TestWindow::Initialize()
{
	D3DApp::Initialize();
	DxSetting setting;
	setting.Width = 1920;
	setting.Height = 1080;
	setting.hWnd = MainWnd();
	setting.FarZ = 10000.0f;
	core = new JGRenderCore;
	core->Init(setting);
	Init = true;
	return true;
}
void TestWindow::OnResize(int width, int height)
{
	if (Init)
	{
		D3DApp::OnResize(width, height);
		core->ScreenReSize(width, height);
	}

}
void TestWindow::Update(const GameTimer& gt)
{
	core->Update(gt);
}
void TestWindow::Draw(const GameTimer& gt)
{
	core->Draw();
}

void TestWindow::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}
void TestWindow::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}
void TestWindow::OnMouseMove(WPARAM btnState, int x, int y)
{
	core->UpdateLook(btnState, x, y, mLastMousePos);
}