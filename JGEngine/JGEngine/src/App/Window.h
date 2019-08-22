#pragma once


#include "JE.h"

class Event;
class KeyCode;
namespace JE
{
	
	class JGENGINE_API Window
	{
	public:
		struct WindowData
		{
			int width, height;
			HWND hWnd;
		};
	public:
		bool Init(HINSTANCE hInst, const wchar_t* appName, 
			int width = 1280, int height = 720);
		const WindowData& GetData() const { return m_Data; }
		void SetData(const WindowData& data) { m_Data = data; }
		static Window* GetWindow() { return ms_WinApp; }
	private:
		static Window* ms_WinApp;
		WindowData m_Data;
	};

}
