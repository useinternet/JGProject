#include"HelloWindow.h"


bool HelloWindow::Initialize()
{
	if (!JGWindow::Initialize())
		return false;
	// 코드 입력


	return true;
}

void HelloWindow::OnResize(int width, int height)
{
	JGWindow::OnResize(width, height);
}
void HelloWindow::Update()
{

}
void HelloWindow::Draw()
{

}

// Convenience overrides for handling mouse input.
void HelloWindow::OnMouseDown(WPARAM btnState, int x, int y)
{

}
void HelloWindow::OnMouseUp(WPARAM btnState, int x, int y)
{

}
void HelloWindow::OnMouseMove(WPARAM btnState, int x, int y)
{

}