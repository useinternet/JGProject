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

	AssetLoadThread = Any & ((~RenderThread) & (~AnimThread) & (~InputThread) & (~GameThread))
};

class PThread : public IMemoryObject
{
	std::thread _thread;
	uint64 _id;

	PQueue<PSharedPtr<PTask>> _taskQueue;

	AtomicBool _bIsActive;
	AtomicBool _bIsRunning;

	PMutex _mutex;
	PConditionVariable _conditionVariable;
public:
	PThread();
	virtual ~PThread();

public:
	template<class ... Args>
	void PushTask(PWeakPtr<IMemoryObject> refObject, const std::function<void(const PTaskArguments&)>& func, const Args& ... args)
	{
		PSharedPtr<PTask> task = Allocate<PTask>();
		task->Bind(PTaskDelegate::Create(refObject, func), args...);

		PLockGuard<PMutex> lock(_mutex);
		_taskQueue.push(task);

		_conditionVariable.notify_one();
	}

	uint64 GetThreadID() const;
	bool IsWaiting() const;
	bool IsRunning() const;

private:
	void init();
	void run();
	void destroy();
};