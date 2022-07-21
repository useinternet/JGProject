#pragma once

#include "CoreDefines.h"


class AGlobalSystemInstanceBase;
class ACoreSystem
{
private:
	static ACoreSystem* Instance;
	static PHashMap<uint64, AGlobalSystemInstanceBase*> SystemInstancePool;
	static PList<ThreadID> ThreadIDList;
private:
	ACoreSystem() = default;
	~ACoreSystem() = default;

public:
	static void Create();
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
		return SystemInstancePool[code];
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

class AGlobalSystemInstanceBase
{
public:
	virtual ~AGlobalSystemInstanceBase() = default;
	
protected:
	virtual void Update() {}
};

template<class T>
class AGlobalSystemInstance : public AGlobalSystemInstanceBase
{
public:
	virtual ~AGlobalSystemInstance() = default;


	static T& GetInstance()
	{
		T* instance = ACoreSystem::GetSystemInstance<T>();
		return *instance;
	}
	static bool IsValid()
	{
		return ACoreSystem::IsValidSystemInstance<T>();
	}
};
