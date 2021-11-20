#include "pch.h"
#include "Window.h"
#include "Class/Global/Scheduler.h"
namespace JG
{
	void IWindow::BindCallBackEventFn(const EventCallBackFn& eventCallBackFn)
	{
		WindowCallBackFn::EventCallBack = eventCallBackFn;
	}
	JRect IWindow::GetClientRect(handle _handle)
	{
		RECT rect = { 0, };
		::GetClientRect((HWND)_handle, &rect);
		
		return JRect(rect.left, rect.top, rect.right, rect.bottom);
	}
	JVector2 IWindow::ClientToScreen(handle _handle, const JVector2& clientPos)
	{
		POINT pt;
		pt.x = clientPos.x;
		pt.y = clientPos.y;
		::ClientToScreen((HWND)_handle, &pt);


		return JVector2(pt.x, pt.y);
	}
	handle IWindow::GetFocus()
	{
		return (handle)::GetFocus();
	}
	JVector2 IWindow::GetCursorPos()
	{
		POINT p;
		::GetCursorPos(&p);
		return JVector2((f32)p.x, (f32)p.y);
	}
	void IWindow::SetCursorPos(const JVector2& pos)
	{
		::SetCursorPos(pos.x, pos.y);
	}
}