#pragma once
#include "Common/Define.h"
#include "Common/Type.h"



namespace JG
{
	class IGlobalSingletonBase
	{
	public:
		virtual ~IGlobalSingletonBase() = default;
	};

	class GlobalSingletonManager
	{
		static GlobalSingletonManager* smGlobalSingletonManager;
	private:
		Dictionary<Type, UniquePtr<IGlobalSingletonBase>> mSingletonManagerPool;

		GlobalSingletonManager()  = default;
		~GlobalSingletonManager() = default;
	public:
		static void Create();
		static void Destroy();
		static GlobalSingletonManager& GetInstance();
		static GlobalSingletonManager* GetInstancePtr();
	private:
		friend class PluginLinker;
		static void SetInstancePtr(GlobalSingletonManager* ptr);
	public:
		template<class T, class ...Args>
		void RegisterSingleton(Args... args)
		{
			Type type = JGTYPE(T);
			if (mSingletonManagerPool.find(type) != mSingletonManagerPool.end())
			{
				return;
			}
			UniquePtr<T> ptr = CreateUniquePtr<T>(args...);
			mSingletonManagerPool.emplace(type, std::move(ptr));
		}
		template<class T>
		void UnRegisterSingleton()
		{
			Type type = JGTYPE(T);
			if (mSingletonManagerPool.find(type) == mSingletonManagerPool.end())
			{
				return;
			}
			mSingletonManagerPool.erase(type);
		}
		template<class T>
		T* GetSingletonInstance()
		{
			Type type = JGTYPE(T);
			if (mSingletonManagerPool.find(type) == mSingletonManagerPool.end())
			{
				return nullptr;
			}
			return static_cast<T*>(mSingletonManagerPool[type].get());
		}

		template<class T>
		bool IsValidSingleton() const 
		{
			
			Type type = JGTYPE(T);
			return mSingletonManagerPool.find(type) != mSingletonManagerPool.end();
		}
	};



	template<class T>
	class IGlobalSingleton : public IGlobalSingletonBase
	{
	public:
		virtual ~IGlobalSingleton() = default;
		static T& GetInstance()
		{
			T* instance = GlobalSingletonManager::GetInstance().GetSingletonInstance<T>();
			return *instance;
		}
		static bool IsValid()
		{
			return GlobalSingletonManager::GetInstance().IsValidSingleton<T>();
		}
	};


}

