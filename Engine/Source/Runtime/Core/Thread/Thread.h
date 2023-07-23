#pragma once
#include "CoreDefines.h"
#include "Memory/Memory.h"
#include "Misc/Delegate.h"
#include "Task.h"
#include <thread>


JG_ENUM_FLAG(ENamedThread)
enum class ENamedThread
{
	Any			 = 0xffffff,
	RenderThread = 0x0001,
	AnimThread   = 0x0002,
	InputThread  = 0x0004,
	GameThread   = 0x0008,
	RemainThread    = Any & ((~RenderThread) & (~AnimThread) & (~InputThread) & (~GameThread)),
	AssetLoadThread = RemainThread,
};

class PThread : public IMemoryObject
{
	std::thread _thread;
	uint64 _id;

	HQueue<PSharedPtr<PTask>> _taskQueue;

	HAtomicBool _bIsActive;
	HAtomicBool _bIsRunning;

	HMutex _mutex;
	HConditionVariable _conditionVariable;
public:
	PThread();
	virtual ~PThread();

public:
	HTaskHandle PushTask(const PTaskDelegate& inTaskDelegate)
	{
		PSharedPtr<PTask> task = PTask::Create(inTaskDelegate);

		HLockGuard<HMutex> lock(_mutex);
		_taskQueue.push(task);

		_conditionVariable.notify_one();

		return task->CreateHandle();
	}

	uint64 GetThreadID() const;
	bool IsWaiting() const;
	bool IsRunning() const;

private:
	void init();
	void run();
	void destroy();

public:
};


inline ThreadID CurrentThreadID()
{
	return std::hash<std::thread::id>()(std::this_thread::get_id());
}