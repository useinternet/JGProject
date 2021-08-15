#pragma once
#include "Events.h"
#include "Common/Type.h"




namespace JG
{
	//CREATE_EVENT_TWO_ARG_(EEventCategory::Mouse, MouseMovedEvent, f64, X, 0, f64, Y, 0, "MouseMovedEvent : [ X : " + std::to_string(X) + ", Y : " + std::to_string(Y) + " ]")

	CREATE_EVENT_ONE_ARG_(EEventCategory::Keyboard | EEventCategory::Input, KeyPressedEvent, EKeyCode, KeyCode, (EKeyCode)0, "KeyPressed : " + KeyCodeToString(KeyCode))
	CREATE_EVENT_ONE_ARG_(EEventCategory::Keyboard | EEventCategory::Input, KeyReleasedEvent, EKeyCode, KeyCode, (EKeyCode)0, "KeyReleasedEvent : " + KeyCodeToString(KeyCode))
	CREATE_EVENT_ONE_ARG_(EEventCategory::Keyboard | EEventCategory::Input, KeyTypingEvent, wchar, Key, L'\0', "KeyTyping : " + Key)
	//class KeyEvent : public IEvent
	//{
	//public:
	//	EKeyCode KeyCode;
	//};


	//class KeyPressedEvent : public KeyEvent
	//{
	//	EVENTCLASS(KeyPressedEvent, EEventCategory::Keyboard | EEventCategory::Input)
	//public:
	//	virtual ~KeyPressedEvent() override  {}
	//	virtual String ToString() const override
	//	{
	//		return ;
	//	}

	//	
	//};

	//class KeyReleasedEvent : public KeyEvent
	//{
	//	EVENTCLASS(KeyReleasedEvent, EEventCategory::Keyboard | EEventCategory::Input)
	//public:
	//	virtual ~KeyReleasedEvent() override  {}
	//	virtual String ToString() const override
	//	{
	//		return "KeyReleased : " + KeyCodeToString(KeyCode);
	//	}
	//};


	//class KeyTypingEvent : public IEvent
	//{
	//	EVENTCLASS(KeyTypingEvent, EEventCategory::Keyboard | EEventCategory::Input)
	//public:
	//	virtual ~KeyTypingEvent() override {}
	//	virtual String ToString() const override
	//	{
	//		return "KeyTyping : " + Key;
	//	}
	//public:
	//	wchar Key;
	//};
	

	
}