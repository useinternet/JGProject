#include "pch.h"
#include "JWindow.h"
#include "CommonCore.h"
using namespace std;

LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

HWND JWindow::GetHandle() const
{
	return m_hWnd;
}
JWindowDesc JWindow::GetDesc()
{
	return m_Desc;
}
void JWindow::SetClientSize(uint32_t width, uint32_t height)
{
	m_Desc.width = width;
	m_Desc.height = height;
	if (m_hWnd != GetFocus())
		return;
	RECT rect;
	GetWindowRect(m_hWnd, &rect);
	SetWindowPos(m_hWnd, HWND_NOTOPMOST, rect.left, rect.top, m_Desc.width, m_Desc.height, SWP_SHOWWINDOW);
}
void JWindow::SetPosition(uint32_t x, uint32_t y)
{
	if (m_hWnd != GetFocus())
		return;

	SetWindowPos(m_hWnd, HWND_NOTOPMOST,
		x, y, m_Desc.width, m_Desc.height, SWP_SHOWWINDOW);
}


bool JWindow::Init(const JWindowDesc& desc, uint32_t startX, uint32_t startY)
{

	m_Desc = desc;
	std::wstring w_name;
	w_name.assign(m_Desc.name.begin(), m_Desc.name.end());
	HINSTANCE hInst = GetModuleHandle(NULL);
	WNDCLASSEXW wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = MainWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = LoadIcon(0, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = w_name.c_str();
	wcex.hIconSm = LoadIcon(0, IDI_APPLICATION);

	RegisterClassExW(&wcex);

	m_hWnd = CreateWindowW(w_name.c_str(), w_name.c_str(), WS_POPUP,
		startX, startY, m_Desc.width, m_Desc.height, 0, nullptr, hInst, nullptr);

	if (!m_hWnd)
		return false;


	ShowWindow(m_hWnd, SW_NORMAL);
	UpdateWindow(m_hWnd);

	return true;
}



std::shared_ptr<JWindowManager> g_WindowManager = nullptr;
HWND g_MainhWnd = 0;
mutex g_WindowMutex;
bool g_isLock = false;
void JWindowManager::Init()
{
	g_WindowManager = make_shared<JWindowManager>();
}

JWindow* JWindowManager::Create(const JWindowDesc& desc, uint32_t startX, uint32_t startY)
{
	if (!g_isLock)
	{
		lock_guard<mutex> lock(g_WindowMutex);
		g_isLock = true;
	}

	if (g_WindowManager->JWindowMapByName.find(desc.name) != g_WindowManager->JWindowMapByName.end())
	{
		g_isLock = false;
		ENGINE_LOG_ERROR("{0} areadly exsit window name", desc.name);
		return nullptr;
	}


	auto window = make_shared<JWindow>();
	if (!window->Init(desc, startX, startY))
	{
		g_isLock = false;
		return nullptr;
	}


	JWindow* p = window.get();

	g_WindowManager->JWindowPool[p] = window;
	g_WindowManager->JWindowMapByName[desc.name] = p;
	g_WindowManager->JWindowMapByHWND[p->GetHandle()] = p;

	if (g_WindowManager->JWindowPool.size() == 1)
		SetMainWindow(p);
	g_isLock = false;
	return p;
}
JWindow* JWindowManager::Find(HWND hWnd)
{
	if (!g_isLock)
	{
		lock_guard<mutex> lock(g_WindowMutex);
		g_isLock = true;
	}
	if (g_WindowManager->JWindowMapByHWND.find(hWnd) == g_WindowManager->JWindowMapByHWND.end())
	{
		g_isLock = false;
		return nullptr;
	}
	g_isLock = false;
	return g_WindowManager->JWindowMapByHWND[hWnd];
}
JWindow* JWindowManager::Find(const std::string& name)
{
	if (!g_isLock)
	{
		lock_guard<mutex> lock(g_WindowMutex);
		g_isLock = true;
	}
	if (g_WindowManager->JWindowMapByName.find(name) == g_WindowManager->JWindowMapByName.end())
	{
		g_isLock = false;
		return nullptr;
	}

	g_isLock = false;
	return g_WindowManager->JWindowMapByName[name];
}
void JWindowManager::Destroy(HWND hWnd)
{
	if (g_WindowManager == nullptr)
		return;
	if (!g_isLock)
	{
		lock_guard<mutex> lock(g_WindowMutex);
		g_isLock = true;
	}
	if (g_WindowManager->JWindowMapByHWND.find(hWnd) == g_WindowManager->JWindowMapByHWND.end())
	{
		return;
	}
	auto p_win = g_WindowManager->JWindowMapByHWND[hWnd];

	g_WindowManager->Destroy(p_win);

	g_isLock = false;
}
void JWindowManager::Destroy(const std::string& name)
{
	if (g_WindowManager == nullptr)
		return;
	if (!g_isLock)
	{
		lock_guard<mutex> lock(g_WindowMutex);
		g_isLock = true;
	}
	if (g_WindowManager->JWindowMapByName.find(name) == g_WindowManager->JWindowMapByName.end())
	{
		return;
	}
	auto p_win = g_WindowManager->JWindowMapByName[name];
	g_WindowManager->Destroy(p_win);

	g_isLock = false;
}
void JWindowManager::Destroy(JWindow* window)
{
	if (g_WindowManager == nullptr)
		return;
	if (!g_isLock)
	{
		lock_guard<mutex> lock(g_WindowMutex);
		g_isLock = true;
	}
	if (g_WindowManager->JWindowPool.find(window) == g_WindowManager->JWindowPool.end())
	{
		return;
	}
	HWND hWnd = window->GetHandle();
	g_WindowManager->JWindowMapByHWND.erase(window->GetHandle());
	g_WindowManager->JWindowMapByName.erase(window->GetDesc().name);
	g_WindowManager->JWindowPool.erase(window);
	DestroyWindow(hWnd);

	g_isLock = false;
}

void JWindowManager::SetMainWindow(JWindow* main)
{
	g_MainhWnd = main->GetHandle();
}

uint32_t JWindowManager::WindowCount()
{
	lock_guard<mutex> lock(g_WindowMutex);
	return g_WindowManager->JWindowPool.size();
}

void JWindowManager::BindWindowProcFunc(const std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>& func)
{
	g_WindowManager->BindedWindowProcFunc = func;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		if (g_MainhWnd == hWnd)
		{
			g_WindowManager.reset();
			PostQuitMessage(0);
		}
		break;
	}
	if (g_WindowManager && g_WindowManager->BindedWindowProcFunc)
	{
		return g_WindowManager->BindedWindowProcFunc(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}