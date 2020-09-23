#include "pch.h"
#include "JWindow.h"
#include "GlobalSharedData.h"
#include "SystemCore.h"
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
	long flag = WS_POPUP | WS_VISIBLE;
	

	m_hWnd = CreateWindowW(w_name.c_str(), w_name.c_str(), flag,
		startX, startY, m_Desc.width, m_Desc.height, desc.parent_hWnd, nullptr, hInst, nullptr);

	if (!m_hWnd)
		return false;


	ShowWindow(m_hWnd, SW_NORMAL);
	UpdateWindow(m_hWnd);

	return true;
}








JWindow* JWindowManager::Create(const JWindowDesc& desc, uint32_t startX, uint32_t startY)
{
	{
		lock_guard<shared_mutex> lock(WindowMutex);
		if (JWindowMapByName.find(desc.name) != JWindowMapByName.end())
		{
			return nullptr;
		}
	}




	auto window = make_shared<JWindow>();
	if (!window->Init(desc, startX, startY))
	{
		return nullptr;
	}


	JWindow* p = window.get();


	{
		lock_guard<shared_mutex> lock(WindowMutex);
		JWindowPool[p] = window;
		JWindowMapByName[desc.name] = p;
		JWindowMapByHWND[p->GetHandle()] = p;
		if (JWindowPool.size() == 1)
			SetMainWindow(p);
	}


	return p;
}
JWindow* JWindowManager::Find(HWND hWnd)
{
	shared_lock<shared_mutex> lock(WindowMutex);
	if (JWindowMapByHWND.find(hWnd) == JWindowMapByHWND.end())
	{
		return nullptr;
	}
	return JWindowMapByHWND[hWnd];
}
JWindow* JWindowManager::Find(const std::string& name)
{

	shared_lock<shared_mutex> lock(WindowMutex);
	if (JWindowMapByName.find(name) == JWindowMapByName.end())
	{
		return nullptr;
	}
	return JWindowMapByName[name];
}
void JWindowManager::Destroy(HWND hWnd)
{
	JWindow* p_win = nullptr;
	{
		shared_lock lock(WindowMutex);
		auto iter = JWindowMapByHWND.find(hWnd);
		if (iter == JWindowMapByHWND.end()) return;
		p_win = iter->second;
	}

	Destroy(p_win);
}
void JWindowManager::Destroy(const std::string& name)
{
	JWindow* p_win = nullptr;
	{
		shared_lock lock(WindowMutex);
		auto iter = JWindowMapByName.find(name);
		if (iter == JWindowMapByName.end()) return;
		p_win = iter->second;
	}
	Destroy(p_win);
}
void JWindowManager::Destroy(JWindow* window)
{
	lock_guard<shared_mutex> lock(WindowMutex);
	if (JWindowPool.find(window) == JWindowPool.end())
	{
		return;
	}
	HWND hWnd = window->GetHandle();
	JWindowMapByHWND.erase(window->GetHandle());
	JWindowMapByName.erase(window->GetDesc().name);
	JWindowPool.erase(window);
	DestroyWindow(hWnd);
}

void JWindowManager::SetMainWindow(JWindow* main)
{
	MainhWnd = main->GetHandle();
}

uint32_t JWindowManager::WindowCount()
{
	shared_lock<shared_mutex> lock(WindowMutex);
	return JWindowPool.size();
}

void JWindowManager::BindWindowProcFunc(SystemCore* core, const std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>& func)
{
	BindedWindowProcFunc[core] = func;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	auto windowManager = GlobalSharedData::GetWindowManager();


	if (windowManager)
	{
		Concurrency::task_group tasks;
		
		for (auto& iter : windowManager->BindedWindowProcFunc)
		{
			tasks.run([&]()
			{
				iter.second(hWnd, msg, wParam, lParam);
			});
		}
		tasks.wait();
	}
	switch (msg)
	{
	case WM_DESTROY:
		if (windowManager->MainhWnd == hWnd)
		{
			PostQuitMessage(0);
		}
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}