#include "CoreSystem.h"
#include "Misc/Log.h"
#include "Memory/Memory.h"
#include "Object/ObjectGlobalSystem.h"
#include "Misc/Scheduler.h"

GCoreSystem* GCoreSystem::Instance = nullptr;
PHashMap<uint64, GGlobalSystemInstanceBase*> GCoreSystem::SystemInstancePool;
PList<ThreadID> GCoreSystem::ThreadIDList;

bool GCoreSystem::Create()
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
	GCoreSystem::RegisterSystemInstance<GScheduleGlobalSystem>();
	GCoreSystem::RegisterSystemInstance<GObjectGlobalSystem>();

	if (GObjectGlobalSystem::GetInstance().codeGen() == false)
	{
		JG_LOG(Core, ELogLevel::Critical, "Fail ObjectGlobalSystem Code Generation");
	}

	for (const PPair<uint64, GGlobalSystemInstanceBase*>& pair : Instance->SystemInstancePool)
	{
		pair.second->Start();
	}

	return true;
}
void GCoreSystem::Update()
{
	for (PPair<const uint64, GGlobalSystemInstanceBase*>& pair : SystemInstancePool)
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

	for (const PPair< uint64, GGlobalSystemInstanceBase*>& pair : Instance->SystemInstancePool)
	{
		pair.second->Destroy();
	}

	GCoreSystem::UnRegisterSystemInstance<GObjectGlobalSystem>();
	GCoreSystem::UnRegisterSystemInstance<GScheduleGlobalSystem>();
	GCoreSystem::UnRegisterSystemInstance<GMemoryGlobalSystem>();
	GCoreSystem::UnRegisterSystemInstance<GLogGlobalSystem>();

	SystemInstancePool.clear();
	ThreadIDList.clear();


	delete Instance;
	Instance = nullptr;
}


uint32 GCoreSystem::GetThreadCount()
{
	static uint32  threadCount = std::thread::hardware_concurrency();
	return threadCount;
}

PList<ThreadID> GCoreSystem::GetAllThreadIDs()
{
	return ThreadIDList;
}

void GCoreSystem::collectionThreadIDs()
{
	PList<std::thread> tempThreads;
	PMutex tempMutex;


	uint32 threadCount = GetThreadCount();
	tempThreads.resize(threadCount);

	for (uint32 i = 0; i < threadCount; ++i)
	{
		tempThreads[i] = std::thread([&]()
		{
			PLockGuard<PMutex> lock(tempMutex);
			ThreadIDList.push_back(std::this_thread::get_id());
		});
	}
	for (uint32 i = 0; i < threadCount; ++i)
	{
		tempThreads[i].join();
	}
}
