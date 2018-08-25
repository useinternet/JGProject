#pragma once
#include"Engine.h"

// HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam

class ENGINE_EXPORT JGInputEvent
{
private:



public:
	JGInputEvent();
	~JGInputEvent();
	void DoEvent(UINT message, WPARAM wParam, LPARAM lParam);
};