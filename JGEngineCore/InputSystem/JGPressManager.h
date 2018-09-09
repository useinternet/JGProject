#pragma once
#include"../EngineStatics/Engine.h"

class JGVector2D;
/*
EnumClass : EKey 
Exp : 키 목록 (Axis가 붙어있는 enum값은 Axis전용 함수를따로 이용하여 바인딩해야한다.)*/
enum class ENGINE_EXPORT EKey
{
	// 숫자 키
	_0, _1, _2, _3, _4, _5, _6, _7, _8, _9,
	// F숫자 키
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	// NumPad 키
	NumLock, NumPad0, NumPad1, NumPad2, NumPad3, NumPad4, NumPad5, NumPad6, NumPad7,
	NumPad8, NumPad9, NumPadEnter,
	// 기능 키
	Escape, Delete, End, Home, Insert, PgDn, PgUp,
	Shift, Ctrl, Alt, CapsLock, Tab,
	// 알파벳 키
	A, B, C, D, E, F, G, H, I, J, K, L, N, M, O, P, Q, R, S, T,
	U, V, W, X, Y, Z,
	// 마우스
	MouseXAxis, MouseYAxis, MouseLButton, MouseRButton,
	MouseWheelClick, MouseWheelAxis
};
/*
Class : JGPressManager
@m KeyBoardState* m_pKeyBoardState : 현제 키보드 상황( 어느키가 눌렸는지..)
@m MouseState* m_pMouseState : 마우스 상태( 마우스 버튼이 눌렀는지.. 움직인정도좌표값 등등)  */
class ENGINE_EXPORT JGPressManager
{
	typedef DIMOUSESTATE  MouseState;
	typedef unsigned char KeyBoardState;
private:
	KeyBoardState* m_pKeyBoardState = nullptr;
	MouseState* m_pMouseState = nullptr;
	JGVector2D* m_pMousePos = nullptr;
public:
	JGPressManager();
	~JGPressManager();
	/*
	Exp : InputSystem 에서 생성한 키보드 및 마우스 장치 연결후 이 클래에다 키보드상태와 마우스상태를 넘겨준다. 
	@param KeyBoardState* state : 키보드상태 주소값
	@param MouseState* MouseSate : 마우스 상태 주소값 
	@param JGVector2D* MousePosPointer : 마우스 좌표 값 */
	void LinkInputSystemKeyBoardState(KeyBoardState* state, MouseState* MouseState,JGVector2D* MousePosPointer);
	/*
	Exp : 해당 키가 눌렀는지 여부*/
	bool IsPressed(const EKey Key);
	/*
	Exp : Axis 입력의 출려값을 가져온다. */
	float GetAxisKey(const EKey Key);

	/*
	Exp : 마우스 좌표를 가져온다. */
	JGVector2D& GetMouseLocation();
};