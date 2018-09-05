#pragma once
#include"Component.h"
#include"../../InputSystem/JGCommandManager.h"
#include"../../InputSystem/JGKeyCommand.h"
#include"../../InputSystem/JGPressManager.h"

class ENGINE_EXPORT InputComponent : public Component
{
private:
	
public:
	InputComponent(const InputComponent& copy) = delete;
	InputComponent();
	~InputComponent();

	/*
	Exp : 키 관련 커맨드를 이벤트 바인딩한다.
	@param const wstring CommandName : 등록한 커맨드 이름
	@param const EKeyState KeyState : 키 상태
	@param const function<void()>& Event : void() 형 바인딩 할 이벤트 */
	void BindKeyCommand(const std::wstring& CommandName,const EKeyState KeyState, const std::function<void()>& Event);
	/*
	Exp : Axist(축) 커맨드를 이벤트 바인딩한다. 
	@param const wstring CommandName : 등록한 커맨드 이름
	@param const function<void(const float)>& Event : void(const float) 형 바인딩할 이벤트 */
	void BindAxisCommand(const std::wstring& CommandName, const std::function<void(const float)>& Event);
};