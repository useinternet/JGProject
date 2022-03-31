#pragma once
#include "Define.h"
#include "Type.h"
#include "Class/Global/GlobalSingletonManager.h"


namespace JG
{
	template<class FactoryClass, class InterfaceClass, i32 BufferCount>
	class ObjectFactory : public IGlobalSingleton<FactoryClass>
	{
	protected:
		Dictionary<InterfaceClass*, SharedPtr<InterfaceClass>> mObjectPool;
		Queue<InterfaceClass*> mReservedDestroyObjectQueue[BufferCount];
		i32 mQueueBufferIndex = 0;
		std::shared_mutex mMutex;
	public:
		ObjectFactory()
		{
			Scheduler::GetInstance().ScheduleByFrame(0, 3, -1, SchedulePriority::DestroyObject, [&]() -> EScheduleResult
			{
				auto& currQueue = mReservedDestroyObjectQueue[mQueueBufferIndex];
				if (currQueue.empty() == false)
				{
					while (currQueue.empty() == false)
					{
						auto destroyObject = currQueue.front(); currQueue.pop();
						DestroyObjectImmediate(destroyObject);
					}
				}
				mQueueBufferIndex = (mQueueBufferIndex + 1) % BufferCount;
				return EScheduleResult::Continue;
			});
		}
		virtual ~ObjectFactory() = default;
	public:
		template<class T>
		T* CreateObject()
		{
			SharedPtr<T> object = CreateSharedPtr<T>();
			{
				std::lock_guard<std::shared_mutex> lock(mMutex);
				mObjectPool[object.get()] = object;
			}
			CreateObjectImpl(object.get());
			return object.get();
		}
		void DestroyObjectImmediate(InterfaceClass* ifClass)
		{
			if (ifClass == nullptr)
			{
				return;
			}
			std::lock_guard<std::shared_mutex> lock(mMutex);
			auto iter = mObjectPool.find(ifClass);
			if (iter != mObjectPool.end())
			{
				DestroyObjectImpl(ifClass);
				mObjectPool.erase(iter);
			}
		}

		void DestroyObject(InterfaceClass* ifClass)
		{
			if (ifClass == nullptr)
			{
				return;
			}
			i32 nextIndex   = (mQueueBufferIndex + 1) % BufferCount;
			auto& nextQueue = mReservedDestroyObjectQueue[nextIndex];
			std::lock_guard<std::shared_mutex> lock(mMutex);
			auto iter = mObjectPool.find(ifClass);
			if (iter != mObjectPool.end())
			{
				ReserveDestroyObjectImpl(ifClass);
				nextQueue.push(ifClass);
			}
		}


		void ForEach(const std::function<void(InterfaceClass*)>& action)
		{
			for (auto& obj : mObjectPool)
			{
				action(obj.second.get());
			}
		}
	protected:
		virtual void CreateObjectImpl(InterfaceClass* ifClass) {};
		virtual void DestroyObjectImpl(InterfaceClass* ifClass) {};
		virtual void ReserveDestroyObjectImpl(InterfaceClass* ifClass) {};
	};
}






