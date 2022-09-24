#include "CoreSystem.h"
#include "Misc/Log.h"
#include "Memory/Memory.h"
#include "Object/ObjectGlobalSystem.h"
#include "Thread/Scheduler.h"
#include "String/StringTable.h"
#include "Misc/Timer.h"
#include "Misc/Module.h"
#include <crtdbg.h>

GCoreSystem* GCoreSystem::Instance = nullptr;
HHashMap<uint64, GGlobalSystemInstanceBase*> GCoreSystem::SystemInstancePool;
HList<ThreadID> GCoreSystem::ThreadIDList;

bool GCoreSystem::Create(ECoreSystemFlags flags)
{
	/* CoreSystem ������ ���

* AScheduler
* * ������ ���� �� �Լ� ���� ����
* * �Ҽ� �ִٸ� C++20 �ڷ�ƾ�� ����
*
* AStringGlobalSystem
* * ���ڿ� ����, �����ڵ� <-> �ƽ�Ű �� ��ȯ �� ��Ʈ�� �ڵ����
*
* AModuleGlobalSystem
* ��� ����
*/

	if (Instance != nullptr)
	{
		return false;
	}

	Instance = new GCoreSystem;

	collectionThreadIDs();

	GCoreSystem::RegisterSystemInstance<GLogGlobalSystem>();
	GCoreSystem::RegisterSystemInstance<GStringTable>();
	GCoreSystem::RegisterSystemInstance<GMemoryGlobalSystem>();
	GCoreSystem::RegisterSystemInstance<GTimerGlobalSystem>();
	GCoreSystem::RegisterSystemInstance<GScheduleGlobalSystem>();
	GCoreSystem::RegisterSystemInstance<GObjectGlobalSystem>();
	GCoreSystem::RegisterSystemInstance<GModuleGlobalSystem>();

	if ((flags & ECoreSystemFlags::No_CodeGen) == false)
	{
		if (GObjectGlobalSystem::GetInstance().codeGen() == false)
		{
			JG_LOG(Core, ELogLevel::Critical, "Fail ObjectGlobalSystem Code Generation");
		}
	}

	for (const HPair<uint64, GGlobalSystemInstanceBase*>& pair : Instance->SystemInstancePool)
	{
		pair.second->Start();
	}

	return true;
}
void GCoreSystem::Update()
{
	for (HPair<const uint64, GGlobalSystemInstanceBase*>& pair : SystemInstancePool)
	{
		pair.second->Update();
	}
}
void GCoreSystem::Destroy()
{
	if (Instance == nullptr)
	{
		return;
	}

	for (const HPair< uint64, GGlobalSystemInstanceBase*>& pair : Instance->SystemInstancePool)
	{
		pair.second->Destroy();
	}

	GCoreSystem::UnRegisterSystemInstance<GModuleGlobalSystem>();
	GCoreSystem::UnRegisterSystemInstance<GObjectGlobalSystem>();
	GCoreSystem::UnRegisterSystemInstance<GStringTable>();
	GCoreSystem::UnRegisterSystemInstance<GScheduleGlobalSystem>();
	GCoreSystem::UnRegisterSystemInstance<GTimerGlobalSystem>();
	GCoreSystem::UnRegisterSystemInstance<GMemoryGlobalSystem>();
	GCoreSystem::UnRegisterSystemInstance<GLogGlobalSystem>();

	SystemInstancePool.clear();
	ThreadIDList.clear();


	delete Instance;
	Instance = nullptr;
	int* ss = new int;
#if _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG
}


uint32 GCoreSystem::GetThreadCount()
{
	static uint32  threadCount = std::thread::hardware_concurrency();
	return threadCount;
}

HList<ThreadID> GCoreSystem::GetAllThreadIDs()
{
	return ThreadIDList;
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
			ThreadIDList.push_back(std::hash<std::thread::id>()(std::this_thread::get_id()));
		});
	}
	for (uint32 i = 0; i < threadCount; ++i)
	{
		tempThreads[i].join();
	}
}
