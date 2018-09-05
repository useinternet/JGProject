#pragma once
#include"../EngineStatics/Engine.h"

class JGPressManager;
class JGCommandManager;


/*
Class : InputSystem
@m IDirectInput8* m_DirectInput : 다이렉트 입력 장치 포인터
@m IDirectInputDevice8* m_KeyBoard : 다이렉트 키보드 장치 포인터
@m IDirectInputDevice8* m_Mouse : 다이렉트 마우스 장치 포인터
@m unique_ptr<JGPressManager> m_PressManager : 키가 눌렸는지 관리하는 클래스
@m unique_ptr<JGCommandManager> m_CommandManager : 키값과 함께 바인딩된 이벤트함수들을 관리하는 클래스
@m MouseState m_MouseState : 마우스 상태
@m KeyBoardState m_KeyBoardState[256] : 키보드 상태 */
class ENGINE_EXPORT InputSystem
{
	typedef unsigned char KeyBoardState;
	typedef DIMOUSESTATE  MouseState;
private:
	IDirectInput8* m_DirectInput;
	IDirectInputDevice8* m_KeyBoard;
	IDirectInputDevice8* m_Mouse;
	std::unique_ptr<JGPressManager> m_PressManager;
	std::unique_ptr<JGCommandManager> m_CommandManager;
	MouseState m_MouseState;
	KeyBoardState m_KeyBoardState[256];

	HWND m_hWnd;
	float m_ScreenWidth;
	float m_ScreenHeight;
public:
	InputSystem();
	~InputSystem();
	/*
	Exp : 입력 장치를 생성한다. 
	@param HINSTANCE hinst : 입력 을 연동시킬 프로그램 인스턴스 
	@param HWND hWnd : 윈도우 핸들
	@param const int width : 클라이언트 가로길이
	@param const int height : 클라이언트 세로길이*/
	bool CreateInputDevice(HINSTANCE hinst, HWND hWnd,const int width,const int height);
	void Tick();


	JGCommandManager* GetCommandManager();
private:
	bool ReadKeyboard();
	bool ReadMouse();

};