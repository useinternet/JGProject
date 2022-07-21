#include "CoreSystem.h"
#include "Memory/Memory.h"
#include "Object/ObjectGlobalSystem.h"

ACoreSystem* ACoreSystem::Instance = nullptr;
PHashMap<uint64, AGlobalSystemInstanceBase*> ACoreSystem::SystemInstancePool;
PList<ThreadID> ACoreSystem::ThreadIDList;

void ACoreSystem::Create()
{
	if (Instance != nullptr)
	{
		return;
	}

	Instance = new ACoreSystem;

	collectionThreadIDs();


	/* CoreSystem 구현할 목록
	
	* AScheduler
	* * 쓰레드 실행 및 함수 실행 관리
	* * 할수 있다면 C++20 코루틴도 도입
	* 
	* AMemoryGlobalSystem
	* * 메모리 관리
	* 
	* AObjectGlobalSystem
	* * 오브젝트 코드 생성 및 프로퍼티 관리
	* 
	* AStringGlobalSystem
	* * 문자열 관리, 유니코드 <-> 아스키 간 변환 및 스트링 코드관리
	* 
	* AModuleGlobalSystem
	* 모듈 관리
	* 
	* ALogGlobalSystem
	* 로그 출력 및 파일 관리
	
	
	*/
	ACoreSystem::RegisterSystemInstance<AMemoryGlobalSystem>();
	ACoreSystem::RegisterSystemInstance<AObjectGlobalSystem>();
}
void ACoreSystem::Update()
{
}
void ACoreSystem::Destroy()
{
	if (Instance == nullptr)
	{
		return;
	}

	ACoreSystem::RegisterSystemInstance<AMemoryGlobalSystem>();
	ACoreSystem::UnRegisterSystemInstance<AObjectGlobalSystem>();


	SystemInstancePool.clear();
	ThreadIDList.clear();


	delete Instance;
	Instance = nullptr;
}


uint32 ACoreSystem::GetThreadCount()
{
	static uint32  threadCount = std::thread::hardware_concurrency();
	return threadCount;
}

PList<ThreadID> ACoreSystem::GetAllThreadIDs()
{
	return ThreadIDList;
}

void ACoreSystem::collectionThreadIDs()
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
