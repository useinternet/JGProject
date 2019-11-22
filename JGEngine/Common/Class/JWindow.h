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

private:
	bool Init(HINSTANCE hinst);
private:
	HWND m_hWnd;
	JWindowDesc m_Desc;

};