#pragma once

#include "CoreDefines.h"
#include "CoreSystem.h"
#include "Thread.h"
#include "Misc/SequentialIDGenerator.h"
#include "MainThreadExecutionOrder.h"

#define SCHEDULE_FN_BIND(func) std::bind(&##func, this, std::placeholders::_1)

class PTimer;

class GScheduleGlobalSystem : public GGlobalSystemInstance<GScheduleGlobalSystem>
{
	enum class ESyncTaskType
	{
		SyncByTick,
		SyncByFrame,
	};

	class ISyncTask : public IMemoryObject
	{
	public:
		uint64 ID;
		int32  Repeat = 0;
		int32  Priority = 0;
		int32  CallCount = 0;
	public:
		virtual ~ISyncTask() = default;
		virtual ESyncTaskType GetTaskType() const = 0;
	};

	class PSyncTaskByTick : public ISyncTask
	{
	public:
		PSharedPtr<PTask> Task;
		float32 Delay     = 0.0f;
		float32 TickCycle = 0.0f;
		float32 Tick      = 0.0f;
	public:
		virtual ~PSyncTaskByTick() = default;
		virtual ESyncTaskType GetTaskType() const { return ESyncTaskType::SyncByTick; }
	};

	class PSyncTaskByFrame : public ISyncTask
	{
	public:
		PSharedPtr<PTask> Task;
		int32 Delay = 0;
		int32 FrameCycle = 0;
		int32 Frame = 0;
	public:
		virtual ~PSyncTaskByFrame() = default;
		virtual ESyncTaskType GetTaskType() const { return ESyncTaskType::SyncByFrame; }
	};

	HList<PSharedPtr<PThread>> _threads;
	HHashMap<ENamedThread, int32> _threadIndexMappingMaps;
	int32 _mappedThreadIndexOffset;
	HAtomicInt32 _remindThreadIndex;

	HHashMap<uint64, PSharedPtr<ISyncTask>>  _syncTaskPool;
	HMap<int32, HList<PWeakPtr<ISyncTask>>>  _sortedSyncTasks;
	HQueue<PWeakPtr<ISyncTask>> _reservedSyncTasks;

	PSharedPtr<PSequentialIDGenerator> _idGenerator;
	bool _bIsTaskRunning;

	PSharedPtr<PTimer> _timer;
public:
	GScheduleGlobalSystem();
	virtual ~GScheduleGlobalSystem() = default;

protected:
	virtual void Start() override;
	virtual void Update() override;
	virtual void Destroy() override;

public:

	uint64 ScheduleOnceByFrame(int32 delayFrame, EMainThreadExecutionOrder order, const PTaskDelegate& inTaskDelegate)
	{
		return ScheduleByFrame(delayFrame, 0, INDEX_NONE, order, inTaskDelegate);
	}

	uint64 ScheduleOnceByFrame(EMainThreadExecutionOrder order, const PTaskDelegate& inTaskDelegate)
	{
		return ScheduleByFrame(0, 0, INDEX_NONE, order, inTaskDelegate);
	}

	uint64 ScheduleByFrame(EMainThreadExecutionOrder order, const PTaskDelegate& inTaskDelegate)
	{
		return ScheduleByFrame(0, 0, INDEX_NONE, order, inTaskDelegate);
	}

	uint64 ScheduleByFrame(int32 delayFrame, EMainThreadExecutionOrder order, const PTaskDelegate& inTaskDelegate)
	{
		return ScheduleByFrame(delayFrame, 0, INDEX_NONE, order, inTaskDelegate);
	}

	uint64 ScheduleByFrame(int32 delayFrame, int32 frameCycle, EMainThreadExecutionOrder order, const PTaskDelegate& inTaskDelegate)
	{
		return ScheduleByFrame(delayFrame, frameCycle, INDEX_NONE, order, inTaskDelegate);
	}

	uint64 ScheduleByFrame(int32 delayFrame, int32 frameCycle, int32 repeat, EMainThreadExecutionOrder order, const PTaskDelegate& inTaskDelegate)
	{
		PSharedPtr<PSyncTaskByFrame> syncTask = Allocate<PSyncTaskByFrame>();

		syncTask->Task = PTask::Create(inTaskDelegate);
		syncTask->ID   = _idGenerator->NewID();
		syncTask->Priority   = (int32)order;
		syncTask->Delay      = delayFrame;
		syncTask->FrameCycle = frameCycle;
		syncTask->Repeat     = repeat;

		_syncTaskPool.emplace(syncTask->ID, syncTask);

		if (_bIsTaskRunning == true)
		{
			_reservedSyncTasks.push(syncTask);
		}
		else
		{
			_sortedSyncTasks[syncTask->Priority].push_back(syncTask);
		}

		return syncTask->ID;
	}

	uint64 ScheduleOnce(float32 delay, EMainThreadExecutionOrder order, const PTaskDelegate& inTaskDelegate)
	{
		return Schedule(delay, 0.0f, INDEX_NONE, order, inTaskDelegate);
	}

	uint64 ScheduleOnce(EMainThreadExecutionOrder order, const PTaskDelegate& inTaskDelegate)
	{
		return Schedule(0.0f, 0.0f, INDEX_NONE, order, inTaskDelegate);
	}

	uint64 Schedule(EMainThreadExecutionOrder order, const PTaskDelegate& inTaskDelegate)
	{
		return Schedule(0.0f, 0.0f, INDEX_NONE, order, inTaskDelegate);
	}

	uint64 Schedule(float32 delay, EMainThreadExecutionOrder order, const PTaskDelegate& inTaskDelegate)
	{
		return Schedule(delay, 0.0f, INDEX_NONE, order, inTaskDelegate);
	}

	uint64 Schedule(float32 delay, float32 tickCycle, EMainThreadExecutionOrder order, const PTaskDelegate& inTaskDelegate)
	{
		return Schedule(delay, tickCycle, INDEX_NONE, order, inTaskDelegate);
	}

	uint64 Schedule(float32 delay, float32 tickCycle, int32 repeat, EMainThreadExecutionOrder order, const PTaskDelegate& inTaskDelegate)
	{
		PSharedPtr<PSyncTaskByTick> syncTask = Allocate<PSyncTaskByTick>();

		syncTask->Task = PTask::Create(inTaskDelegate);
		syncTask->ID   = _idGenerator->NewID();
		syncTask->Priority  = (int32)order;
		syncTask->Delay     = delay;
		syncTask->TickCycle = tickCycle;
		syncTask->Repeat    = repeat;

		_syncTaskPool.emplace(syncTask->ID, syncTask);

		if (_bIsTaskRunning == true)
		{
			_reservedSyncTasks.push(syncTask);
		}
		else
		{
			_sortedSyncTasks[syncTask->Priority].push_back(syncTask);
		}

		return syncTask->ID;
	}

	HTaskHandle ScheduleAsync(ENamedThread namedThread, const PTaskDelegate& inTaskDelegate)
	{
		int32 fixedThreadIndex = getRecommandThreadIndex(namedThread);

		PSharedPtr<PThread> thread = _threads[fixedThreadIndex];

		return thread->PushTask(inTaskDelegate);
	}
	
private:
	bool updateTask(PSyncTaskByFrame* task);
	bool updateTask(PSyncTaskByTick* task);

	void  assignNamedThread();
	int32 getRecommandThreadIndex(ENamedThread namedThread);
};