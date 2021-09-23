#pragma once
#include "JGCore.h"
#include <functional>

namespace JG
{
	enum class WindowCallBackResult
	{
		
	};
	
	struct WindowProps
	{
		const wchar_t* TitleName = nullptr;
		u32 Width  = 0;
		u32 Height = 0;
	};

	class WindowCallBackFn
	{
	private:
		static std::function<void(IEvent&)> EventCallBack;
	public:
		static std::function<EventResult()>										WindowOpenCallBack;
		static std::function<EventResult()>										WindowCloseCallBack;
		static std::function<EventResult(i32, i32)>								WindowResizeCallBack;
		static std::function<EventResult(EKeyCode, EInputAction, EInputMode)>		WindowKeyInputCallBack;
		static std::function<EventResult(EMouseButton, EInputAction, EInputMode)>	WindowMouseButtonCallBack;
		static std::function<EventResult(wchar)>									WindowCharCallBack;
		static std::function<EventResult(double, double)>							WindowScrollCallBack;
		static std::function<EventResult(double, double)>							WndowMouseMoveCallBack;
	private:
		friend class IWindow;
		static void Bind();
		
	};
	class IWindow
	{
		
	public:
		using EventCallBackFn = std::function<void(IEvent&)>;
		IWindow()
		{
			WindowCallBackFn::Bind();
		}
		virtual ~IWindow(){}
		virtual bool Create(const WindowProps& props) = 0;
		virtual bool Update()  = 0;
		virtual void Destroy() = 0;


		virtual EWindowPlatform GetPlatform() const = 0;
		virtual handle GetHandle() const   = 0;
		virtual JVector2Int GetPosition() const = 0;
		static void BindCallBackEventFn(const EventCallBackFn& eventCallBackFn)
		{
			WindowCallBackFn::EventCallBack = eventCallBackFn;
		}
	};
}




