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


	/* CoreSystem ������ ���
	
	* AScheduler
	* * ������ ���� �� �Լ� ���� ����
	* * �Ҽ� �ִٸ� C++20 �ڷ�ƾ�� ����
	* 
	* AMemoryGlobalSystem
	* * �޸� ����
	* 
	* AObjectGlobalSystem
	* * ������Ʈ �ڵ� ���� �� ������Ƽ ����
	* 
	* AStringGlobalSystem
	* * ���ڿ� ����, �����ڵ� <-> �ƽ�Ű �� ��ȯ �� ��Ʈ�� �ڵ����
	* 
	* AModuleGlobalSystem
	* ��� ����
	* 
	* ALogGlobalSystem
	* �α� ��� �� ���� ����
	
	
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
