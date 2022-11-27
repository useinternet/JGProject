#include "PCH/PCH.h"
#include "WindowsJWindow.h"

LRESULT CALLBACK WndProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam);

HJWHandle PWindowsJWindow::GetHandle() const
{
	return _handle;
}

void PWindowsJWindow::ConstructWindow(const HJWindowArguments& args)
{
	_handle = 0;

	HRawWString wTitle = args.Title.GetRawWString();

	HJModule hInstance = GetModuleHandle(NULL);

	WNDCLASSEXW wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style  = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance  = hInstance;

	if (args.IconID == -1) wcex.hIcon = LoadIcon(0, IDI_APPLICATION);
	else wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(args.IconID));

	if (args.IconID == -1) wcex.hIconSm = LoadIcon(0, IDI_APPLICATION);
	else wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(args.IconID));

	wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName  = 0;
	wcex.lpszClassName = wTitle.c_str();

	RegisterClassExW(&wcex);

	_handle = CreateWindowW(
		wcex.lpszClassName, wcex.lpszClassName,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, args.Size.x, args.Size.y, nullptr, nullptr,
		hInstance, nullptr);
	// WS_OVERLAPPEDWINDOW
	// WS_POPUP | WS_VISIBLE
	if (_handle == 0)
	{
		return;
	}

	ShowWindow(_handle, SW_NORMAL);
	UpdateWindow(_handle);
}

void PWindowsJWindow::AddWindowProcCallBack(const WindowsWindowProcCallBack& callBackFunc)
{
	_sCallBackList.push_back(callBackFunc);
}

LRESULT WndProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam)
{
	for (WindowsWindowProcCallBack& callBack : PWindowsJWindow::_sCallBackList)
	{
		LRESULT result = callBack(hWnd, msg, wParam, lParam);
		if (result)
		{
			return result;
		}
	}
	switch (msg)
	{
	//case WM_MOUSEWHEEL:
	//	//return WindowCallBackFn::WindowScrollCallBack((f64)GET_WHEEL_DELTA_WPARAM(wParam) / (f64)WHEEL_DELTA, 0);
	//case WM_MOUSEHWHEEL:
	//	//return WindowCallBackFn::WindowScrollCallBack(0, (f64)GET_WHEEL_DELTA_WPARAM(wParam) / (f64)WHEEL_DELTA);
	//case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
	//case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
	//case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
	//case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
	//{
	//	/*EMouseButton mBtn;
	//	if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) { mBtn = EMouseButton::Left; }
	//	if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) { mBtn = EMouseButton::Right; }
	//	if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) { mBtn = EMouseButton::Middle; }
	//	if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK) { mBtn = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? EMouseButton::XButton1 : EMouseButton::XButton2; }
	//	return WindowCallBackFn::WindowMouseButtonCallBack(mBtn, EInputAction::Pressed, EInputMode::Default);*/
	//}
	//case WM_LBUTTONUP:
	//case WM_RBUTTONUP:
	//case WM_MBUTTONUP:
	//case WM_XBUTTONUP:
	//{
	//	/*EMouseButton mBtn;
	//	if (msg == WM_LBUTTONUP) { mBtn = EMouseButton::Left; }
	//	if (msg == WM_RBUTTONUP) { mBtn = EMouseButton::Right; }
	//	if (msg == WM_MBUTTONUP) { mBtn = EMouseButton::Middle; }
	//	if (msg == WM_XBUTTONUP) { mBtn = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? EMouseButton::XButton1 : EMouseButton::XButton2; }
	//	return WindowCallBackFn::WindowMouseButtonCallBack(mBtn, EInputAction::Released, EInputMode::Default);*/
	//}
	//return 0;
	//case WM_KEYDOWN:
	//case WM_SYSKEYDOWN:
	//	//return WindowCallBackFn::WindowKeyInputCallBack((EKeyCode)wParam, EInputAction::Pressed, EInputMode::Default);
	//case WM_KEYUP:
	//case WM_SYSKEYUP:
	//	//return WindowCallBackFn::WindowKeyInputCallBack((EKeyCode)wParam, EInputAction::Released, EInputMode::Default);
	//case WM_CHAR:
	//	//return WindowCallBackFn::WindowCharCallBack(wParam);
	//case WM_SIZE:
	//	//return WindowCallBackFn::WindowResizeCallBack(LOWORD(lParam), HIWORD(lParam));
	//	//case WM_SETCURSOR:
	//	//	if (LOWORD(lParam) == HTCLIENT) return 1;
	//	//	return 0;
	//case WM_CREATE:
	//	//return WindowCallBackFn::WindowOpenCallBack();
	//	return DefWindowProc(hWnd, msg, wParam, lParam);
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
