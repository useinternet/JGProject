#pragma once
#include"Engine.h"

// HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam
class InputSystem;
class ENGINE_EXPORT JGInputEvent
{
private:
	InputSystem* m_pInputSystem;


public:
	JGInputEvent();
	~JGInputEvent();
	void LinkInputSystem(InputSystem* input);
	void DoEvent(UINT message, WPARAM wParam, LPARAM lParam);
};