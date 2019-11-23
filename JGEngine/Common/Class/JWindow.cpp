#include "pch.h"
#include "JWindow.h"

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

	RECT rect;
	GetWindowRect(m_hWnd, &rect);
	SetWindowPos(m_hWnd, HWND_TOP, rect.left, rect.top, m_Desc.width, m_Desc.height, SWP_SHOWWINDOW);
}
void JWindow::SetPosition(uint32_t x, uint32_t y)
{
	SetWindowPos(m_hWnd, HWND_TOP,
		x, y, m_Desc.width, m_Desc.height, SWP_SHOWWINDOW);
}

void JWindow::AddClientSize(uint32_t width, uint32_t height)
{
	m_Desc.width  += width;
	m_Desc.height += height;

	RECT rect;
	GetWindowRect(m_hWnd, &rect);
	SetWindowPos(m_hWnd, HWND_TOP, rect.left, rect.top, m_Desc.width, m_Desc.height, SWP_SHOWWINDOW);
}


void JWindow::AddPosition(uint32_t x, uint32_t y)
{
	RECT rect;
	GetWindowRect(m_hWnd, &rect);
	SetWindowPos(m_hWnd, HWND_TOP, 
		         rect.left + x, rect.top + y,
		         m_Desc.width, m_Desc.height, SWP_SHOWWINDOW);
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
		startX, startY, m_Desc.width, m_Desc.height, nullptr, nullptr, hInst, nullptr);

	if (!m_hWnd)
		return false;


	ShowWindow(m_hWnd, SW_NORMAL);
	UpdateWindow(m_hWnd);

	return true;
}



JWindowManager* g_WindowManager = nullptr;

JWindowManager::JWindowManager()
{
	if (g_WindowManager == nullptr)
		g_WindowManager = this;
}
JWindowManager::~JWindowManager()
{
	g_WindowManager = nullptr;
}
JWindow* JWindowManager::Create(const JWindowDesc& desc, uint32_t startX, uint32_t startY)
{
	if (g_WindowManager->JWindowMapByName.find(desc.name) != g_WindowManager->JWindowMapByName.end())
		return nullptr;

	auto window = make_shared<JWindow>();
	if (!window->Init(desc, startX, startY))
	{
		return nullptr;
	}


	JWindow* p = window.get();

	g_WindowManager->JWindowPool[p] = window;
	g_WindowManager->JWindowMapByName[desc.name] = p;
	g_WindowManager->JWindowMapByHWND[p->GetHandle()] = p;

	return p;
}
JWindow* JWindowManager::Find(HWND hWnd)
{
	if (g_WindowManager->JWindowMapByHWND.find(hWnd) == g_WindowManager->JWindowMapByHWND.end())
	{
		return nullptr;
	}
	return g_WindowManager->JWindowMapByHWND[hWnd];
}
JWindow* JWindowManager::Find(const std::string& name)
{
	if (g_WindowManager->JWindowMapByName.find(name) == g_WindowManager->JWindowMapByName.end())
	{
		return nullptr;
	}
	return g_WindowManager->JWindowMapByName[name];
}
void JWindowManager::Destroy(HWND hWnd)
{
	if (g_WindowManager->JWindowMapByHWND.find(hWnd) == g_WindowManager->JWindowMapByHWND.end())
	{
		return;
	}
	auto p_win = g_WindowManager->JWindowMapByHWND[hWnd];

	g_WindowManager->Destroy(p_win);
}
void JWindowManager::Destroy(const std::string& name)
{
	if (g_WindowManager->JWindowMapByName.find(name) == g_WindowManager->JWindowMapByName.end())
	{
		return;
	}
	auto p_win = g_WindowManager->JWindowMapByName[name];
	g_WindowManager->Destroy(p_win);
}
void JWindowManager::Destroy(JWindow* window)
{
	if (g_WindowManager->JWindowPool.find(window) == g_WindowManager->JWindowPool.end())
	{
		return;
	}
	HWND hWnd = window->GetHandle();
	g_WindowManager->JWindowMapByHWND.erase(window->GetHandle());
	g_WindowManager->JWindowMapByName.erase(window->GetDesc().name);
	g_WindowManager->JWindowPool.erase(window);


	DestroyWindow(hWnd);
}




LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}