#include "pch.h"
#include "WindowsWindow.h"
#include "Application.h"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace JG
{
	List<WindowProcCallBack> WindowsWindow::smCallBackList;
	LRESULT CALLBACK WndProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam);


	bool WindowsWindow::Create(const WindowProps& props)
	{
		mProps = props;

		auto hInstance = GetModuleHandle(NULL);
		WNDCLASSEXW wcex = {};
		wcex.cbSize		 = sizeof(WNDCLASSEX);
		wcex.style       = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra  = 0;
		wcex.cbWndExtra  = 0;
		wcex.hInstance   = hInstance;
		if (props.IconID == -1) wcex.hIcon = LoadIcon(0, IDI_APPLICATION);
		else wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(props.IconID));
		if (props.IconID == -1) wcex.hIconSm = LoadIcon(0, IDI_APPLICATION);
		else wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(props.IconID));
		wcex.hCursor     = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName  = 0;
		wcex.lpszClassName = mProps.TitleName;

		RegisterClassExW(&wcex);
		
		mHandle = CreateWindowW(
			mProps.TitleName, mProps.TitleName,
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, mProps.Width, mProps.Height, nullptr, nullptr,
			hInstance, nullptr);
		// WS_OVERLAPPEDWINDOW
		// WS_POPUP | WS_VISIBLE
		if (mHandle == 0)
		{
			return false;
		}

		ShowWindow(mHandle, SW_NORMAL);
		UpdateWindow(mHandle);
		return true;
	}

	bool WindowsWindow::Update()
	{
		MSG msg;
		while(::PeekMessage(&msg, NULL, 0,0, PM_REMOVE))
		{
			if(msg.message != WM_QUIT)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				AppCloseEvent e;
				Application::GetInstance().SendEventImmediate(e);
				return false;
			}
		}
		return true;
	}

	void WindowsWindow::Destroy()
	{
		
	}


	EWindowPlatform WindowsWindow::GetPlatform() const
	{
		return EWindowPlatform::Windows;
	}
	handle WindowsWindow::GetHandle() const
	{
		return (handle)mHandle;
	}

	JVector2Int WindowsWindow::GetPosition() const
	{
		RECT rect;
		::GetWindowRect(mHandle, &rect);
		return JVector2Int(rect.left, rect.top);
	}

	void WindowsWindow::SetShowCursor(bool isShow)
	{
		mIsShowCursor = isShow;
		if (mIsShowCursor)
		{
			while (::ShowCursor(true) <= 0) {}
		}
		else
		{
			while (::ShowCursor(false) >= 0) {}
		}
	}

	bool WindowsWindow::IsShowCursor() const
	{
		return mIsShowCursor;
	}

	void WindowsWindow::AddWindowProcCallBack(const WindowProcCallBack& callBack)
	{
		smCallBackList.push_back(callBack);
	}



	LRESULT WndProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam)
	{
		for (auto& callBack : WindowsWindow::smCallBackList)
		{
			LRESULT result = callBack(hWnd, msg, wParam, lParam);
			if (result)
			{
				return result;
			}
		}
		switch (msg)
		{
		case WM_MOUSEWHEEL:
			return WindowCallBackFn::WindowScrollCallBack((f64)GET_WHEEL_DELTA_WPARAM(wParam) / (f64)WHEEL_DELTA, 0);
		case WM_MOUSEHWHEEL:
			return WindowCallBackFn::WindowScrollCallBack(0, (f64)GET_WHEEL_DELTA_WPARAM(wParam) / (f64)WHEEL_DELTA);
		case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
		case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
			{
				EMouseButton mBtn;
				if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) { mBtn = EMouseButton::Left; }
				if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) { mBtn = EMouseButton::Right; }
				if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) { mBtn = EMouseButton::Middle; }
				if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK) { mBtn = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? EMouseButton::XButton1 : EMouseButton::XButton2; }
				return WindowCallBackFn::WindowMouseButtonCallBack(mBtn, EInputAction::Pressed, EInputMode::Default);
			}
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
			{
				EMouseButton mBtn;
				if (msg == WM_LBUTTONUP) { mBtn = EMouseButton::Left; }
				if (msg == WM_RBUTTONUP) { mBtn = EMouseButton::Right; }
				if (msg == WM_MBUTTONUP) { mBtn = EMouseButton::Middle; }
				if (msg == WM_XBUTTONUP) { mBtn = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? EMouseButton::XButton1 : EMouseButton::XButton2; }
				return WindowCallBackFn::WindowMouseButtonCallBack(mBtn, EInputAction::Released, EInputMode::Default);
			}
			return 0;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			return WindowCallBackFn::WindowKeyInputCallBack((EKeyCode)wParam, EInputAction::Pressed, EInputMode::Default);
		case WM_KEYUP:
		case WM_SYSKEYUP:
			return WindowCallBackFn::WindowKeyInputCallBack((EKeyCode)wParam, EInputAction::Released, EInputMode::Default);
		case WM_CHAR:
			return WindowCallBackFn::WindowCharCallBack(wParam);
		case WM_SIZE:
			return WindowCallBackFn::WindowResizeCallBack(LOWORD(lParam), HIWORD(lParam));
		//case WM_SETCURSOR:
		//	if (LOWORD(lParam) == HTCLIENT) return 1;
		//	return 0;
		case WM_CREATE:
			return WindowCallBackFn::WindowOpenCallBack();
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

