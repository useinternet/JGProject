#include "pch.h"
#include "Window.h"
#include "Class/Log.h"
#include "Event/WindowEvents.h"
#include "GUI/EditorGUI/ImGui/imgui.h"
#include "GUI/EditorGUI/ImGui/imgui_impl_win32.h"
using namespace std;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
namespace JE
{
	Window* Window::ms_WinApp = nullptr;
	bool Window::Init(HINSTANCE hInst, const wchar_t* appName, int width, int height)
	{

		ms_WinApp = this;
		m_Data.width = width;
		m_Data.height = height;

		// 윈도우 양식 등록
		WNDCLASSEXW wcex = {};
		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = LoadIcon(0, IDI_APPLICATION);
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = 0;
		wcex.lpszClassName = appName;
		wcex.hIconSm = LoadIcon(0, IDI_APPLICATION);

		RegisterClassExW(&wcex);

		m_Data.hWnd = CreateWindowW(appName, appName, WS_POPUP ,
			CW_USEDEFAULT, 0, m_Data.width, m_Data.height, nullptr, nullptr, hInst, nullptr);

		if (!m_Data.hWnd)
		{
			return false;
		}

		ShowWindow(m_Data.hWnd, SW_NORMAL);
		UpdateWindow(m_Data.hWnd);
		return true;
	}
}
void WindowReSizeFunc(int width, int height)
{
	auto win = JE::Window::GetWindow();
	WindowResizeEvent e;
	e.width = width;
	e.height = height;
	
	auto data   = win->GetData();
	data.width  = width;
	data.height = height;
	win->SetData(data);

	GlobalLinkData::OnEvent(e);
}
extern IMGUI_IMPL_API LRESULT  ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static int curr_width  = 0;
	static int curr_height = 0;
	auto data = JE::Window::GetWindow()->GetData();


	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;


	switch (msg)
	{
	case WM_SIZE:
		curr_width  = LOWORD(lParam);
		curr_height = HIWORD(lParam);
		if (data.width != curr_width || data.height != curr_height)
		{
			WindowReSizeFunc(curr_width, curr_height);
		}
		break;
	case WM_INPUT:
	
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}