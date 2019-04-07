#pragma once
#include"JGWindow.h"



class HelloWindow : public JGWindow
{
public:
	HelloWindow(HINSTANCE hInst) : JGWindow(hInst) {}
	virtual bool Initialize();
protected:
	virtual void OnResize(int width, int height) override;
	virtual void Update() override;
	virtual void Draw() override;

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;
};