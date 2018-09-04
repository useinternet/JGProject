#pragma once
#include"JGCommand.h"


/*
EnumClass : EKeyState 
@e Down : 키가 눌렸다.!
@e Up   : 키를 똇다!!
@e None : 평상지!! */
enum class ENGINE_EXPORT EKeyState
{
	Down,
	Up,
	None
};
/*
Class : JGKeyCommand 
@m EKeyState m_KeyState : 키 상태
@m EKeyState m_EventTriggerKeyState : 이벤트를 발생시킬 키 조건..
@m std::function<void()> m_BindEvent : 바인딩된 이벤트
@m bool m_bKeyVariance : 키상태에 변화가 있었는지 여부
@m bool m_PrevPressed  : 이전 키누름상태 */
class ENGINE_EXPORT JGKeyCommand : public JGCommand
{
private:
	EKeyState m_KeyState;
	EKeyState m_EventTriggerKeyState;
	std::function<void()> m_BindEvent;
	bool m_bKeyVariance = false;
	bool m_PrevPressed  = false;
public:
	JGKeyCommand(const JGKeyCommand& copy) = delete;
	JGKeyCommand();
	virtual ~JGKeyCommand();
	virtual void Tick(const float DeltaTime) override;

	/*
	Exp : JGKeyCommand에 키상태와 이벤트(함수)를 바인딩한다. (함수의 형식은 = void() 형식이다.)
	@param : const EKeyState KeyState : 바인딩할 트리거용 키상태
	@param : const function<void()>& Event : void() 형 함수 (클래스멤버변수일경우에는 std::bind 함수를 이용하자 ) */
	void BindKeyCommand(const EKeyState KeyState, const std::function<void()>& Event);
};