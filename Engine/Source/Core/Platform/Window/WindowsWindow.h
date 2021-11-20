#pragma once

#include "Window.h"



namespace JG
{
	using WindowProcCallBack = std::function<LRESULT(HWND, u32, WPARAM wParam, LPARAM lParam)>;
	class WindowsWindow : public IWindow
	{
		static List<WindowProcCallBack> smCallBackList;
	private:
		WindowProps mProps;
		HWND mHandle = 0;
		EventCallBackFn mEventFn;
		bool mIsShowCursor = true;
	public:
		virtual ~WindowsWindow() override {}
	public:
		virtual bool Create(const WindowProps& props) override;
		virtual bool Update() override;
		virtual void Destroy() override;


		virtual EWindowPlatform GetPlatform() const override;
		virtual handle			GetHandle() const override;
		virtual JVector2Int		GetPosition() const override;
		virtual void SetShowCursor(bool isShow) override;
		virtual bool IsShowCursor() const override;


		static void AddWindowProcCallBack(const WindowProcCallBack& callBack);
	private:
		friend 	LRESULT WndProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam);
	};


}

