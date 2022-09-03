#include "Scheduler.h"
#include "Misc/Log.h"

GScheduleGlobalSystem::GScheduleGlobalSystem()
{
	PHashSet<uint64> threadIDHashSet;

	uint32 threadCount = GCoreSystem::GetThreadCount();
	for (uint32 i = 0; i < threadCount; ++i)
	{
		_thread.push_back(Allocate<PThread>());

		threadIDHashSet.insert(_thread[i]->GetThreadID());

		JG_LOG(Core, ELogLevel::Info, "Active Thread: [%u]", _thread[i]->GetThreadID());
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

void GScheduleGlobalSystem::ScheduleByFrame(PWeakPtr<IMemoryObject> refObject, int32 delayFrame, int32 frameCycle, int32 repeat, int32 priority, const std::function<void()>& func)
{
}

void GScheduleGlobalSystem::Schedule(PWeakPtr<IMemoryObject> refObject, float32 delay, float32 tickCycle, int32 repeat, int32 priority, const std::function<void()>& func)
{
}

void GScheduleGlobalSystem::ScheduleAsync(PWeakPtr<IMemoryObject> refObject, const std::function<void()>& func)
{
}
