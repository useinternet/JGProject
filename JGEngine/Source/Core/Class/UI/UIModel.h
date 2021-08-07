#pragma once
#include "Application.h"


namespace JG
{
	class IEvent;
	class IUIModel
	{
		friend class UIViewModel;
	protected:
		virtual void Initialize() = 0;
		virtual void Destroy() = 0;
	public:
		virtual ~IUIModel() = default;
	};
	// 보통 데이터 정의
	class UIModel : public IUIModel
	{
	public:
		virtual ~UIModel() = default;
	protected:
		template<class T>
		void SendEvent(T& e)
		{
			Application::GetInstance().SendEvent(e);
		}
	};
}