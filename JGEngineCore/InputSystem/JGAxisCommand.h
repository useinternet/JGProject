#pragma once
#include"JGCommand.h"

enum class EKey;

/*
Class : JGAxisCommand 
@m function<void(const float)> m_BindEvent : void(const float) 형 바인딩된 이벤트 함수
@m bool bIsBindEvent :  이벤트가 바인딩 되었는지 여부 */
class ENGINE_EXPORT JGAxisCommand : public JGCommand
{
private:
	std::function<void(const float)> m_BindEvent;
	bool bIsBindEvent = false;
public:
	JGAxisCommand();
	virtual ~JGAxisCommand();

	virtual void Tick();
	/*
	Exp : JGAxisCommand에 void(const float) 형태의 함수로 바인딩한다. */
	void BindAxisCommand(const std::function<void(const float)>& Event);

	void KillCommand();
};