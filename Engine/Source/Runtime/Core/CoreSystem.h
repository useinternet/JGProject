#pragma once

#include "CoreDefines.h"


class GGlobalSystemInstanceBase;
class GCoreSystem
{
private:
	static GCoreSystem* Instance;
	static PHashMap<uint64, GGlobalSystemInstanceBase*> SystemInstancePool;
	static PList<ThreadID> ThreadIDList;
private:
	GCoreSystem() = default;
	~GCoreSystem() = default;

public:
	static bool Create();
	static void Update();
	static void Destroy();

	template<class T, class ...Args>
	static void RegisterSystemInstance(Args... args)
	{

		if (IsValidSystemInstance<T>() == true)
		{
			return;
		}

		uint64 code = getTypeHashCode<T>();
		SystemInstancePool.emplace(code, new T(args...));
	}
	template<class T>
	static void UnRegisterSystemInstance()
	{
		if (IsValidSystemInstance<T>() == false)
		{
			return;
		}

		uint64 code = getTypeHashCode<T>();
		T* instance = static_cast<T*>(SystemInstancePool[code]);

		delete instance;
		instance = nullptr;

		SystemInstancePool.erase(code);
	}
	template<class T>
	static T* GetSystemInstance()
	{
		if (IsValidSystemInstance<T>() == false)
		{
			return nullptr;
		}
		uint64 code = getTypeHashCode<T>();
		return static_cast<T*>(SystemInstancePool[code]);
	}

	template<class T>
	static bool IsValidSystemInstance()
	{
		uint64 code = getTypeHashCode<T>();

		return SystemInstancePool.find(code) != SystemInstancePool.end();
	}

	static uint32 GetThreadCount();
	static PList<ThreadID> GetAllThreadIDs();
	
private:
	template<class T>
	static uint64 getTypeHashCode()
	{
		return typeid(T).hash_code();
	}

	static void collectionThreadIDs();
};

class GGlobalSystemInstanceBase
{
	friend GCoreSystem;
public:
	virtual ~GGlobalSystemInstanceBase() = default;
	
protected:
	virtual void Start()   {}
	virtual void Update()  {}
	virtual void Destroy() {}
};

template<class T>
class GGlobalSystemInstance : public GGlobalSystemInstanceBase
{
public:
	virtual ~GGlobalSystemInstance() = default;


	static T& GetInstance()
	{
		T* instance = GCoreSystem::GetSystemInstance<T>();
		return *instance;
	}
	static bool IsValid()
	{
		return GCoreSystem::IsValidSystemInstance<T>();
	}
};
