#pragma once
#include "JGCore.h"
#include "Application.h"



namespace JG
{
	// 
	class IUIModel;
	class IUIError;
	class IUIErrorReceiver;
	class IUIViewModel : public ISubscriber
	{
	private:
		template<class T>
		friend class UIView;
	protected:
		virtual void Initialize() = 0;
		virtual void Destroy()    = 0;
		virtual void OnEvent(IEvent& e) = 0;

	protected:
		virtual void SetErrorReceiver(IUIErrorReceiver* receiver) = 0;
	public:
		virtual ~IUIViewModel() = default;
	};

	// UI모델을 알고있다. UI모델과 상호작용하면서 지속적으로  View에 필요한 정보를 가공해준다.


	class UIViewModel : public IUIViewModel
	{
	private:
		Dictionary<Type, SharedPtr<IUIModel>> mUIModelPool;
		IUIErrorReceiver* mUIErrorReceiver = nullptr;
	protected:
		template<class UIModelType>
		UIModelType* RegisterUIModel()
		{
			Type type = Type(TypeID<UIModelType>());
			if (mUIModelPool.find(type) == mUIModelPool.end())
			{
				mUIModelPool[type] = CreateSharedPtr<UIModelType>();
			}
			mUIModelPool[type]->Initialize();
			return static_cast<UIModelType*>(mUIModelPool[type].get());
		}

		template<class UIModelType>
		UIModelType* GetUIModel() const
		{
			Type type = Type(TypeID<UIModelType>());
			auto iter = mUIModelPool.find(type);
			if (iter == mUIModelPool.end())
			{
				return nullptr;
			}
			return static_cast<UIModelType*>(iter->second.get());
		}
		template<class EventClass>
		void SendEvent(EventClass& e)
		{
			Application::GetInstance().SendEvent(e);
		}
		template<class EventClass>
		void SendEventImmediate(EventClass& e)
		{
			Application::GetInstance().SendEventImmediate(e);
		}
		virtual void Destroy() override;
		virtual void OnEvent(IEvent& e);
	protected:
		void SendError(SharedPtr<IUIError> error);
	private:
		virtual void SetErrorReceiver(IUIErrorReceiver* receiver) override;
	public:
		virtual ~UIViewModel() = default;
	};
}