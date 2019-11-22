#include "pch.h"
#include "JWindow.h"

using namespace std;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

bool JWindow::Init(uint32_t width, uint32_t height, uint32_t startX, uint32_t startY)
{
	HINSTANCE hInst = GetModuleHandle(NULL);
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
	wcex.lpszClassName = m_Desc.name.c_str();
	wcex.hIconSm = LoadIcon(0, IDI_APPLICATION);

	RegisterClassExW(&wcex);

	m_hWnd = CreateWindowW(m_Desc.name.c_str(), m_Desc.name.c_str(), WS_POPUP,
		CW_USEDEFAULT, 0, m_Desc.width, m_Desc.height, nullptr, nullptr, hInst, nullptr);

	if (!m_hWnd)
		return false;


	ShowWindow(m_hWnd, SW_NORMAL);
	UpdateWindow(m_hWnd);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_SIZE:
		break;
	case WM_INPUT:

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}