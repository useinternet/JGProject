#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include <memory>

struct JWindowDesc
{
	std::string name = "JWindow";
	uint32_t width   = 0;
	uint32_t height  = 0;
	HWND parent_hWnd = 0;
};


class JWindow
{
public:
	HWND GetHandle() const;
	JWindowDesc GetDesc();
	void SetClientSize(uint32_t width, uint32_t height);
	void SetPosition(uint32_t x, uint32_t y);
	bool Init(const JWindowDesc& desc, uint32_t startX, uint32_t startY);
private:
	HWND m_hWnd;
	JWindowDesc m_Desc;

};



class JWindowManager
{
	friend LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	static void Init();
	static JWindow* Create(const JWindowDesc& desc, uint32_t startX = 0, uint32_t startY = 0);
	static JWindow* Find(HWND hWnd);
	static JWindow* Find(const std::string& name);

	static void Destroy(HWND hWnd);
	static void Destroy(const std::string& name);
	static void Destroy(JWindow* window);

	static void SetMainWindow(JWindow* main);
	static uint32_t WindowCount();
	static void BindWindowProcFunc(const std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>& func);
private:
	std::unordered_map<JWindow*, std::shared_ptr<JWindow>> JWindowPool;
	std::unordered_map<std::string, JWindow*> JWindowMapByName;
	std::unordered_map<HWND, JWindow*>        JWindowMapByHWND;
	std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> BindedWindowProcFunc;
};