#pragma once
#include "Events.h"





namespace JG
{
	class KeyEvent : public IEvent
	{
	public:
		EKeyCode KeyCode;
	};


	class KeyPressedEvent : public KeyEvent
	{
		EVENTCLASS(EEventCategory::Keyboard | EEventCategory::Input)
	public:
		virtual ~KeyPressedEvent() override  {}
		virtual String ToString() const override
		{
			return "KeyPressed : " + KeyCodeToString(KeyCode);
		}

		
	};

	class KeyReleasedEvent : public KeyEvent
	{
		EVENTCLASS(EEventCategory::Keyboard | EEventCategory::Input)
	public:
		virtual ~KeyReleasedEvent() override  {}
		virtual String ToString() const override
		{
			return "KeyReleased : " + KeyCodeToString(KeyCode);
		}
	};


	class KeyTypingEvent : public IEvent
	{
		EVENTCLASS(EEventCategory::Keyboard | EEventCategory::Input)
	public:
		virtual ~KeyTypingEvent() override {}
		virtual String ToString() const override
		{
			return "KeyTyping : " + Key;
		}
	public:
		wchar Key;
	};
	

	
}