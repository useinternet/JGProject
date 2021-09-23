#include "pch.h"
#include "Window.h"


namespace JG
{
	std::function<void(IEvent&)>									    WindowCallBackFn::EventCallBack;
	std::function<EventResult()>										WindowCallBackFn::WindowOpenCallBack;
	std::function<EventResult()>										WindowCallBackFn::WindowCloseCallBack;
	std::function<EventResult(i32, i32)>								WindowCallBackFn::WindowResizeCallBack;
	std::function<EventResult(EKeyCode, EInputAction, EInputMode)>		WindowCallBackFn::WindowKeyInputCallBack;
	std::function<EventResult(EMouseButton, EInputAction, EInputMode)>	WindowCallBackFn::WindowMouseButtonCallBack;
	std::function<EventResult(wchar)>									WindowCallBackFn::WindowCharCallBack;
	std::function<EventResult(double, double)>							WindowCallBackFn::WindowScrollCallBack;
	std::function<EventResult(double, double)>							WindowCallBackFn::WndowMouseMoveCallBack;

	void WindowCallBackFn::Bind()
	{
		WindowOpenCallBack = [&]()
		{
			AppOpenEvent e;
			EventCallBack(e);
			return e.Result;
		};
		WindowCloseCallBack = [&]()
		{
			AppCloseEvent e;
			EventCallBack(e);

			return e.Result;
		};

		WindowResizeCallBack = [&](i32 width, i32 height)
		{
			AppResizeEvent e;
			e.Width  = width;
			e.Height = height;
			EventCallBack(e);
			return e.Result;
		};

		WindowKeyInputCallBack = [&](EKeyCode keyCode, EInputAction inputAction, EInputMode inputMode)
		{
			EventResult result = 0;
			switch(inputAction)
			{
			case EInputAction::Pressed:
				{
					KeyPressedEvent e;
					e.KeyCode = keyCode;
					EventCallBack(e);
					result = e.Result;
				}
				break;

			case EInputAction::Released:
				{
					KeyReleasedEvent e;
					e.KeyCode = keyCode;
					EventCallBack(e);
					result = e.Result;
				}
				break;
			}
			return result;
		};

		WindowCharCallBack = [&](wchar str)
		{
			KeyTypingEvent e;
			e.Key = str;

			EventCallBack(e);
			return e.Result;
		};

		WindowMouseButtonCallBack = [&](EMouseButton mouseBt, EInputAction inputAction, EInputMode inputMode)
		{
			EventResult result = 0;
			switch (inputAction)
			{
			case EInputAction::Pressed:
			{
				MouseButtonPressedEvent e;
				e.MouseButton = mouseBt;
				EventCallBack(e);
				result =  e.Result;
			}
			break;

			case EInputAction::Released:
			{
				MouseButtonReleasedEvent e;
				e.MouseButton = mouseBt;
				EventCallBack(e);
				result =  e.Result;
			}
			break;
			}
			return result;
		};


		WindowScrollCallBack = [&](f64 offset, f64 hoffset)
		{
			MouseScrollMovedEvent e;
			e.Offset  = offset;
			e.HOffset = hoffset;
			EventCallBack(e);
			return e.Result;
		};

		WndowMouseMoveCallBack = [&](f64 xPos, f64 yPos)
		{
			MouseMovedEvent e;
			e.X = xPos;
			e.Y = yPos;
			EventCallBack(e);
			return e.Result;
		};
	}
}