#include "Scheduler.h"
#include "MainThreadExecutionOrder.h"
#include "Misc/Log.h"


GScheduleGlobalSystem::GScheduleGlobalSystem()
{
	HHashSet<uint64> threadIDHashSet;

	uint32 threadCount = GCoreSystem::GetThreadCount();
	for (uint32 i = 0; i < threadCount; ++i)
	{
		_threads.push_back(Allocate<PThread>());

		threadIDHashSet.insert(_threads[i]->GetThreadID());

		JG_LOG(Core, ELogLevel::Info, "Active Thread: [%u]", _threads[i]->GetThreadID());
	}

	if (threadIDHashSet.size() != threadCount)
	{
		JG_LOG(Core, ELogLevel::Critical, "Duplicate Thread ID");
		JG_ASSERT(false && "Duplicate Thread ID");
	}
	else
	{
		JG_LOG(Core, ELogLevel::Info, "Active Thread Totoal Count: %d ", threadCount);
	}

	assignNamedThread();

	_idGenerator = Allocate<PSequentialIDGenerator>();
	_bIsRunning = false;
}

void GScheduleGlobalSystem::Start()
{
}

void GScheduleGlobalSystem::Update()
{
}

void GScheduleGlobalSystem::Destroy()
{

}

void GScheduleGlobalSystem::RemoveSchedule(uint64 id)
{
}

void GScheduleGlobalSystem::assignNamedThread()
{
	_mappedThreadIndexOffset = 0;

	uint32 threadCount = GCoreSystem::GetThreadCount();
	uint32 prevThreadIndex = 0;

	_threadIndexMappingMaps[ENamedThread::RenderThread] = 0;						
	prevThreadIndex = _threadIndexMappingMaps[ENamedThread::RenderThread];

	_threadIndexMappingMaps[ENamedThread::AnimThread]   = threadCount > 0 ? 1 : prevThreadIndex;
	prevThreadIndex = _threadIndexMappingMaps[ENamedThread::AnimThread];

	_threadIndexMappingMaps[ENamedThread::InputThread]  = threadCount > 1 ? 2 : prevThreadIndex;
	prevThreadIndex = _threadIndexMappingMaps[ENamedThread::InputThread];

	_threadIndexMappingMaps[ENamedThread::GameThread]   = threadCount > 2 ? 3 : prevThreadIndex;
	prevThreadIndex = _threadIndexMappingMaps[ENamedThread::GameThread];

	_mappedThreadIndexOffset = threadCount > 3 ? 4 : prevThreadIndex;
	_threadIndexMappingMaps[ENamedThread::RemainThread] = INDEX_NONE;
}
