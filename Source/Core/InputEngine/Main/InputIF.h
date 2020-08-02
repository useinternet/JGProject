#pragma once


#ifdef INPUTENGINE_EXPORT

#define INPUTENGINE_API __declspec(dllexport)

#else

#define INPUTENGINE_API __declspec(dllimport)

#endif

#include "CommonCore.h"

namespace IE
{
	class InputEngine;
}
class INPUTENGINE_API InputIF
{
	friend IE::InputEngine;
public:
	static JVector2 GetCursorPos();
	static bool GetMouseButtonDown(int bt);
	static bool GetMouseButton(int bt);
	static bool GetMouseButtonUp(int bt);
	static bool GetKeyDown(KeyCode code);
	static bool GetKey(KeyCode code);
	static bool GetKeyUp(KeyCode code);
private:
	void UpdateKeyState();
private:
	enum KeyState
	{
		KeyState_None,
		KeyState_Down,
		KeyState_Hold,
		KeyState_Up
	};
	enum 
	{
		Max_Key_Count = 256
	};
	KeyState m_KeyboardStates[Max_Key_Count];




};