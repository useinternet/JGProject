#pragma once
#include"../EngineStatics/Engine.h"

class JGPressManager;


class ENGINE_EXPORT InputSystem
{
	typedef unsigned char KeyBoardState;
	typedef DIMOUSESTATE  MouseState;
private:
	IDirectInput8* m_DirectInput;
	IDirectInputDevice8* m_KeyBoard;
	IDirectInputDevice8* m_Mouse;
	std::unique_ptr<JGPressManager> m_PressManager;

	MouseState m_MouseState;
	KeyBoardState m_KeyBoardState[256];

	HWND m_hWnd;
	float m_ScreenWidth;
	float m_ScreenHeight;
public:
	InputSystem();
	~InputSystem();

	bool CreateInputDevice(HINSTANCE hinst, HWND hWnd, int width, int height);
	void Tick(const float DeltaTime);

private:
	bool ReadKeyboard();
	bool ReadMouse();

};