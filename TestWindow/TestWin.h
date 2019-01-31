#pragma once
#include"../d3dApp.h"
#include"DxCore/JGRenderCore.h"
#include"DxCore/DxCore.h"
#ifdef _DEBUG
#pragma comment(lib,"JGRenderCore_d.lib")
#else
#pragma comment(lib,"JGRenderCore.lib")
#endif

using namespace JGRC;
class TestWindow : public D3DApp
{
private:
	JGRenderCore* core = nullptr;
	IF_Scene scene;
	IF_Camera cam;
	POINT mLastMousePos;

	bool Init = false;
public:
	TestWindow(HINSTANCE hInst);
	~TestWindow();
	virtual bool Initialize();

protected:
	virtual void OnResize(int width, int height) override;
	virtual void Update(const GameTimer& gt);
	virtual void Draw(const GameTimer& gt);

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y);
	virtual void OnMouseUp(WPARAM btnState, int x, int y);
	virtual void OnMouseMove(WPARAM btnState, int x, int y);
private:
	void InputCamera(const GameTimer& gt);
};