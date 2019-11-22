#pragma once
#include <Windows.h>
#include <iostream>
#include <string>

struct JWindowDesc
{
	std::wstring name = L"JWindow";
	uint32_t width   = 0;
	uint32_t height  = 0;
};


class JWindow
{
public:
	HWND GetHandle() const;
	JWindowDesc GetDesc();
	void SetClientSize(uint32_t width, uint32_t height);
	void SetPosition(uint32_t x, uint32_t y);

	void AddClientSize(uint32_t width, uint32_t height);
	void AddPosition(uint32_t x, uint32_t y);
private:
	bool Init(uint32_t width, uint32_t height, uint32_t startX, uint32_t startY);
private:
	HWND m_hWnd;
	JWindowDesc m_Desc;

};