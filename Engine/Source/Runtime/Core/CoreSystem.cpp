#include "PCH/PCH.h"
#include "CoreSystem.h"
#include "Misc/Log.h"
#include "Memory/Memory.h"
#include "Object/ObjectGlobalSystem.h"
#include "Thread/Scheduler.h"
#include "String/StringTable.h"
#include "Misc/Timer.h"
#include "Misc/Module.h"
#include "Platform/JWindow.h"
#include <crtdbg.h>

void HCoreSystemPrivate::SetInstance(GCoreSystem* instance)
{
	GCoreSystem::Instance = instance;
}

HCoreSystemGlobalValues::HCoreSystemGlobalValues()
{
	GraphicsAPI = nullptr;
	MainWindow = nullptr;
	WindowCallBacks = std::make_unique<HWindowCallBacks>();
}

GCoreSystem* GCoreSystem::Instance = nullptr;

bool GCoreSystem::Create(const HCoreSystemArguments& args)
{
	/* CoreSystem 구현할 목록

* AScheduler
* * 쓰레드 실행 및 함수 실행 관리
* * 할수 있다면 C++20 코루틴도 도입
*
* AStringGlobalSystem
* * 문자열 관리, 유니코드 <-> 아스키 간 변환 및 스트링 코드관리
*
* AModuleGlobalSystem
* 모듈 관리
*/

	if (Instance != nullptr)
	{
		return false;
	}

	Instance = new GCoreSystem;

	collectionThreadIDs();

	GCoreSystem::RegisterSystemInstance<GLogGlobalSystem>();
	GCoreSystem::RegisterSystemInstance<GMemoryGlobalSystem>();
	GCoreSystem::RegisterSystemInstance<GTimerGlobalSystem>();
	GCoreSystem::RegisterSystemInstance<GScheduleGlobalSystem>();
	GCoreSystem::RegisterSystemInstance<GStringTable>();
	GCoreSystem::RegisterSystemInstance<GModuleGlobalSystem>();
	GCoreSystem::RegisterSystemInstance<GObjectGlobalSystem>();

	if ((args.Flags & ECoreSystemFlags::No_CodeGen) == false)
	{
		if (GObjectGlobalSystem::GetInstance().codeGen() == false)
		{
			JG_LOG(Core, ELogLevel::Critical, "Fail ObjectGlobalSystem Code Generation");
		}
	}

	if (args.LaunchModule.length() > 0)
	{
		if (GModuleGlobalSystem::GetInstance().ConnectModule(args.LaunchModule.c_str()) == false)
		{
			JG_LOG(Core, ELogLevel::Error, "Fail Launch Module:%s", args.LaunchModule.c_str());
		}
	}

	for (GGlobalSystemInstanceBase* SystemInstance : Instance->SystemInstanceList)
	{
		SystemInstance->Start();
	}

	Instance->bIsRunning = true;

	return true;
}
bool GCoreSystem::Update()
{
	for (GGlobalSystemInstanceBase* SystemInstance : Instance->SystemInstanceList)
	{
		SystemInstance->Update();
	}

	return Instance->bIsRunning;
}
void GCoreSystem::Destroy()
{
	if (Instance == nullptr)
	{
		return;
	}

	Instance->GlobalValues.MainWindow = nullptr;
	Instance->GlobalValues.GraphicsAPI = nullptr;
	Instance->GlobalValues.WindowCallBacks = nullptr;

	int32 NumSystem = (int32)Instance->SystemInstanceList.size();
	for (int32 i = NumSystem - 1; i >= 0; --i)
	{
		Instance->SystemInstanceList[i]->Destroy();
	}

	GCoreSystem::UnRegisterSystemInstance<GStringTable>();
	GCoreSystem::UnRegisterSystemInstance<GObjectGlobalSystem>();
	GCoreSystem::UnRegisterSystemInstance<GModuleGlobalSystem>();
	GCoreSystem::UnRegisterSystemInstance<GScheduleGlobalSystem>();
	GCoreSystem::UnRegisterSystemInstance<GTimerGlobalSystem>();
	GCoreSystem::UnRegisterSystemInstance<GMemoryGlobalSystem>();
	GCoreSystem::UnRegisterSystemInstance<GLogGlobalSystem>();

	for (HJInstance dllIns : Instance->DllInstances)
	{
		HPlatform::UnLoadDll(dllIns);
	}

	Instance->DllInstances.clear();
	Instance->SystemInstancePool.clear();
	Instance->ThreadIDList.clear();

	delete Instance;
	Instance = nullptr;

#if _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG
}

HCoreSystemGlobalValues& GCoreSystem::GetGlobalValues()
{
	return Instance->GlobalValues;
}

GCoreSystem& GCoreSystem::GetInstance()
{
	return *Instance;
}

void GCoreSystem::RegisterDll(HJInstance InInstance)
{
	Instance->DllInstances.insert(InInstance);
}

void GCoreSystem::UnregisterDll(HJInstance InInstance)
{
	Instance->DllInstances.erase(InInstance);
}

uint32 GCoreSystem::GetThreadCount()
{
	static uint32  threadCount = std::thread::hardware_concurrency();
	return threadCount;
}

HList<ThreadID> GCoreSystem::GetAllThreadIDs()
{
	return Instance->ThreadIDList;
}

void GCoreSystem::collectionThreadIDs()
{
	HList<std::thread> tempThreads;
	HMutex tempMutex;


	uint32 threadCount = GetThreadCount();
	tempThreads.resize(threadCount);

	for (uint32 i = 0; i < threadCount; ++i)
	{
		tempThreads[i] = std::thread([&]()
		{
			HLockGuard<HMutex> lock(tempMutex);
			Instance->ThreadIDList.push_back(std::hash<std::thread::id>()(std::this_thread::get_id()));
		});
	}
	for (uint32 i = 0; i < threadCount; ++i)
	{
		tempThreads[i].join();
	}
}
