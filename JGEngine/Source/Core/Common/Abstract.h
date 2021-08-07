#pragma once
#include "Define.h"
#include "Type.h"
namespace JG
{


	template<class T, class ...Args>
	class GlobalSingleton
	{
	private:
		static T* smInstance;
	protected:
		static void Create(Args... args)
		{
			if (smInstance != nullptr) return;

			smInstance = new T(args...);
		}
		static void Destroy()
		{
			if (smInstance == nullptr) return;
			delete smInstance;
			smInstance = nullptr;
		}
	public:
		static bool IsValid()
		{
			return smInstance != nullptr;
		}
		static T& GetInstance() {
			return *smInstance;
		}
	};


	template<class T, class ...Args>
	T* GlobalSingleton<T, Args...>::smInstance = nullptr;


	//template<class T>
	//class CustomWeakPtr
	//{
	//protected:
	//	T*            mPtr = nullptr;
	//	WeakPtr<bool> mIsValid;
	//public:
	//	CustomWeakPtr(T* ptr) : mPtr(ptr)
	//	{
	//		ConstructorImpl();
	//	}
	//	CustomWeakPtr(const CustomWeakPtr& copy)
	//	{
	//		mPtr = copy.mPtr;
	//		mIsValid = copy.mIsValid;
	//	}
	//	CustomWeakPtr& operator=(const CustomWeakPtr& copy)
	//	{
	//		mPtr = copy.mPtr;
	//		mIsValid = copy.mIsValid;
	//		return *this;
	//	}
	//	T* operator->()
	//	{
	//		return mPtr;
	//	}
	//	~CustomWeakPtr()
	//	{
	//		DestructorImpl();
	//	}
	//	bool IsValid() const
	//	{
	//		auto _bool = mIsValid.lock();
	//		if (_bool)
	//		{
	//			return *(_bool.get());
	//		}
	//		return false;
	//	}
	//private:
	//	CustomWeakPtr(CustomWeakPtr&& rhs) = delete;
	//	CustomWeakPtr& operator=(CustomWeakPtr&& rhs) = delete;

	//protected:
	//	//bool 
	//	virtual void ConstructorImpl() = 0;
	//	virtual void DestructorImpl() = 0;
	//};




	template<class FactoryClass, class InterfaceClass, i32 BufferCount>
	class ObjectFactory : public GlobalSingleton<FactoryClass>
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
				if (currQueue.empty())
				{
					return EScheduleResult::Continue;
				}


				while (currQueue.empty() == false)
				{
					auto destroyObject = currQueue.front(); currQueue.pop();
					DestroyObjectImmediate(destroyObject);
				}

				mQueueBufferIndex = (mQueueBufferIndex + 1) % BufferCount;
				return EScheduleResult::Continue;
			});
		}
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






