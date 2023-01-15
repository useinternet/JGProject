#pragma once

#include "CoreDefines.h"
#include "Platform/PlatformDefines.h"

JG_ENUM_FLAG(ECoreSystemFlags)
enum class ECoreSystemFlags
{
	None = 0x000,
	No_CodeGen = 0x001
};

class GGlobalSystemInstanceBase;
class GCoreSystem;
class PJGGraphicsAPI;
class PJWindow;
class IGUIBuild;
struct HWindowCallBacks;

namespace HCoreSystemPrivate
{
	void SetInstance(GCoreSystem* instance);
}

struct HCoreSystemArguments
{
	ECoreSystemFlags Flags;
	HRawString LaunchModule;


	HCoreSystemArguments() : Flags(ECoreSystemFlags::None) {}
};

struct HCoreSystemGlobalValues
{
	PJGGraphicsAPI* GraphicsAPI;
	PJWindow*		MainWindow;
	IGUIBuild*		GUIBuild;
	HSTLSharedPtr<HWindowCallBacks> WindowCallBacks;
	
	HCoreSystemGlobalValues();
};

class GCoreSystem
{
	friend void HCoreSystemPrivate::SetInstance(GCoreSystem* instance);
private:
	static GCoreSystem* Instance;
	HHashMap<uint64, GGlobalSystemInstanceBase*> SystemInstancePool;
	HList<ThreadID> ThreadIDList;

	HCoreSystemGlobalValues GlobalValues;
public:
	bool bIsRunning;

private:
	GCoreSystem() = default;
	~GCoreSystem() = default;

public:
	static bool Create(const HCoreSystemArguments& args = HCoreSystemArguments());
	static bool Update();
	static void Destroy();
	static HCoreSystemGlobalValues& GetGlobalValues();
	static GCoreSystem& GetInstance();

	template<class T, class ...Args>
	static void RegisterSystemInstance(Args... args)
	{
		if (IsValidSystemInstance<T>() == true)
		{
			return;
		}

		uint64 code = getTypeHashCode<T>();
		Instance->SystemInstancePool.emplace(code, new T(args...));
	}
	template<class T>
	static void UnRegisterSystemInstance()
	{
		if (IsValidSystemInstance<T>() == false)
		{
			return;
		}

		uint64 code = getTypeHashCode<T>();
		GGlobalSystemInstanceBase*& instance = Instance->SystemInstancePool[code];

		delete instance;
		instance = nullptr;

		Instance->SystemInstancePool.erase(code);
	}
	template<class T>
	static T* GetSystemInstance()
	{
		if (IsValidSystemInstance<T>() == false)
		{
			return nullptr;
		}
		uint64 code = getTypeHashCode<T>();
		return static_cast<T*>(Instance->SystemInstancePool[code]);
	}

	template<class T>
	static bool IsValidSystemInstance()
	{
		uint64 code = getTypeHashCode<T>();

		return Instance->SystemInstancePool.find(code) != Instance->SystemInstancePool.end();
	}

	static uint32 GetThreadCount();
	static HList<ThreadID> GetAllThreadIDs();
	
	
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
protected:
	GGlobalSystemInstanceBase() = default;
	virtual ~GGlobalSystemInstanceBase() = default;
	
protected:
	virtual void Start()   {}
	virtual void Update()  {}
	virtual void Destroy() {}
};

template<class T>
class GGlobalSystemInstance : public GGlobalSystemInstanceBase
{
	friend GCoreSystem;

protected:
	GGlobalSystemInstance() = default;
	virtual ~GGlobalSystemInstance() = default;

public:
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


