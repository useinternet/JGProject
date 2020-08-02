#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <shared_mutex>


struct JWindowDesc
{
	std::string name = "JWindow";
	uint32_t width   = 0;
	uint32_t height  = 0;
	HWND parent_hWnd = 0;
};

class JWindowManager;
class SystemCore;
class JWindow
{
	friend JWindowManager;
public:
	HWND GetHandle() const;
	JWindowDesc GetDesc();
	void SetClientSize(uint32_t width, uint32_t height);
	void SetPosition(uint32_t x, uint32_t y);
private:
	bool Init(const JWindowDesc& desc, uint32_t startX, uint32_t startY);
private:
	HWND m_hWnd;
	JWindowDesc m_Desc;

};



class JWindowManager
{
	friend LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	JWindow* Create(const JWindowDesc& desc, uint32_t startX = 0, uint32_t startY = 0);
	JWindow* Find(HWND hWnd);
	JWindow* Find(const std::string& name);

	void Destroy(HWND hWnd);
	void Destroy(const std::string& name);
	void Destroy(JWindow* window);

	void SetMainWindow(JWindow* main);
	HWND GetMainWindowHWND() const {
		return MainhWnd;
	}
	uint32_t WindowCount();
	void BindWindowProcFunc(SystemCore*, const std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>& func);
private:
	using WndProcFunc = std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>;
private:
	std::unordered_map<JWindow*, std::shared_ptr<JWindow>> JWindowPool;
	std::unordered_map<std::string, JWindow*> JWindowMapByName;
	std::unordered_map<HWND, JWindow*>        JWindowMapByHWND;
	std::unordered_map<SystemCore*, WndProcFunc> BindedWindowProcFunc;


	HWND MainhWnd = 0;
	std::shared_mutex WindowMutex;
};