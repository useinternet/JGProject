#include "PCH/PCH.h"
#include "Thread.h"

PThread::PThread()
{
	init();
}

PThread::~PThread()
{
	destroy();
}

uint64 PThread::GetThreadID() const
{
	return _id;
}

bool PThread::IsWaiting() const
{
	return _bIsRunning == false;
}

bool PThread::IsRunning() const
{
	return _bIsRunning == true;
}

void PThread::init()
{
	_thread = std::thread([&]() { run(); });
	_id     = std::hash<std::thread::id>()(_thread.get_id());

	_bIsActive  = true;
	_bIsRunning = false;
}

void PThread::run()
{
	while (_bIsActive || _taskQueue.empty() == false)
	{
		std::unique_lock<std::mutex> lock(_mutex);

		_bIsRunning = _taskQueue.empty() == false;

		_conditionVariable.wait(lock, [&]()->bool
		{
			return _taskQueue.empty() == false || _bIsActive == false;
		});

		if (_taskQueue.empty() == false)
		{
			PSharedPtr<PTask> task = _taskQueue.front(); _taskQueue.pop();
			
			lock.unlock();

			if (task != nullptr && task->IsValid() == true)
			{
				task->DoTask();
			}
		}
	}
}

void PThread::destroy()
{
	{
		HLockGuard<HMutex> lock(_mutex);
		_bIsActive = false;
		_conditionVariable.notify_all();
	}

	_thread.join();
}


