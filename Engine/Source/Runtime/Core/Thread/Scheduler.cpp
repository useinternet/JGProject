#include "PCH/PCH.h"
#include "Scheduler.h"
#include "MainThreadExecutionOrder.h"
#include "Misc/Log.h"
#include "Misc/Timer.h"

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
	_bIsTaskRunning = false;
	_remindThreadIndex = _mappedThreadIndexOffset;
}

void GScheduleGlobalSystem::Start()
{
	_timer = PTimer::Create();
	_timer->Start();
}

void GScheduleGlobalSystem::Update()
{
	_bIsTaskRunning = true;

	for (HPair<const int32, HList<PWeakPtr<ISyncTask>>>& pair : _sortedSyncTasks)
	{
		const int32 id = pair.first;

		HList<PWeakPtr<ISyncTask>>& tasks = pair.second;

		uint64 taskCount = tasks.size();
		for (uint64 i = 0; i < taskCount;)
		{
			bool bIsRemoveTask = true;

			PSharedPtr<ISyncTask> task = tasks[i].Pin();
			
			if (task.IsValid() == true)
			{
				ESyncTaskType taskType = task->GetTaskType();

				switch (taskType)
				{
				case ESyncTaskType::SyncByFrame:
					if (updateTask(static_cast<PSyncTaskByFrame*>(task.GetRawPointer())) == true)
					{
						bIsRemoveTask = false;
					}
					break;

				case ESyncTaskType::SyncByTick:
					if (updateTask(static_cast<PSyncTaskByTick*>(task.GetRawPointer())) == true)
					{
						bIsRemoveTask = false;
					}
					break;
				}
			}

			if (bIsRemoveTask == false)
			{
				++i;
			}
			else
			{
				PWeakPtr<ISyncTask> temp = tasks[i];

				tasks[i] = tasks[taskCount - 1];
				tasks[taskCount - 1] = temp;

				tasks.pop_back();
				taskCount = tasks.size();
			}

		}
	}

	_bIsTaskRunning = false;

	while (_reservedSyncTasks.empty() == false)
	{
		PSharedPtr<ISyncTask> reservedTask = _reservedSyncTasks.front().Pin(); _reservedSyncTasks.pop();
		if (reservedTask.IsValid() == false)
		{
			continue;
		}

		_sortedSyncTasks[reservedTask->Priority].push_back(reservedTask);
	}
}

void GScheduleGlobalSystem::Destroy()
{
	// Flush
}

bool GScheduleGlobalSystem::updateTask(PSyncTaskByFrame* task)
{
	if (task == nullptr)
	{
		return false;
	}
	
	if (task->Task == nullptr || task->Task->IsValid() == false)
	{
		return false;
	}

	if (task->Repeat != INDEX_NONE && task->CallCount > task->Repeat)
	{
		return false;
	}

	if (task->Delay <= task->Frame && task->FrameCycle <= task->Frame)
	{
		task->Task->DoTask();

		task->CallCount += 1;
		task->Frame = 0;
		task->Delay = 0;
	}

	task->Frame += 1;

	return true;
}

bool GScheduleGlobalSystem::updateTask(PSyncTaskByTick* task)
{
	if (task == nullptr)
	{
		return false;
	}

	if (task->Task == nullptr || task->Task->IsValid() == false)
	{
		return false;
	}

	if (task->Repeat != INDEX_NONE && task->CallCount > task->Repeat)
	{
		return false;
	}

	if (task->Delay <= task->Tick && task->TickCycle <= task->Tick)
	{
		task->Task->DoTask();

		task->CallCount += 1;
		task->Tick  = 0.0f;
		task->Delay = 0.0f;
	}

	task->Tick += _timer->GetTick();

	return true;
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

int32 GScheduleGlobalSystem::getRecommandThreadIndex(ENamedThread inNamedThread)
{
	// 처음 돌때는 현재 대기중인 thread 할당
	int32 fixedThreadIndex = INDEX_NONE;
	int32 threadCount = (int32)_threads.size();
	int32 remindThreadCount = threadCount - _mappedThreadIndexOffset;

	if (_mappedThreadIndexOffset == threadCount)
	{
		fixedThreadIndex = threadCount - 1;
	}
	else
	{
		for (const HPair<ENamedThread, int32>& pair : _threadIndexMappingMaps)
		{
			ENamedThread namedThread = pair.first;
			int32 threadIndex = pair.second;

			if ((namedThread & inNamedThread) == false)
			{
				continue;
			}

			if (threadIndex == INDEX_NONE)
			{
				for (int32 i = _mappedThreadIndexOffset; i < threadCount; ++i)
				{
					PSharedPtr<PThread> thread = _threads[i];
					if (thread == nullptr || thread->IsRunning())
					{
						continue;
					}

					fixedThreadIndex = i;
					break;
				}

				if (fixedThreadIndex == INDEX_NONE)
				{
					fixedThreadIndex = _mappedThreadIndexOffset + (_remindThreadIndex % remindThreadCount);
					
					_remindThreadIndex += 1;
				}
			}
			else
			{
				fixedThreadIndex = threadIndex;
			}

			break;
		}
	}

	return fixedThreadIndex;
}
