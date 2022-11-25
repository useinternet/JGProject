#include "PCH/PCH.h"
#include "Task.h"

PTask::PTask()
{
	_bIsRunning = false;
}

bool PTask::IsRunning() const
{
	return _bIsRunning;
}

bool PTask::IsWaiting() const
{
	return _bIsRunning == false;
}

bool PTask::IsValid() const
{
	return _taskDelegate.IsBound();
}

void PTask::DoTask()
{
	_bIsRunning = true;

	_taskDelegate.ExecuteIfBound(_taskArguments);

	_bIsRunning = false;
}

HTaskHandle PTask::CreateHandle() const
{
	return HTaskHandle(SharedWrap(this), &_bIsRunning);
}

HTaskHandle::HTaskHandle(PSharedPtr<PTask> inTask, const HAtomicBool* pInIsRunning)
{
	task = inTask;
	pIsRunning = pInIsRunning;
}

bool HTaskHandle::IsValid() const
{
	return task.IsValid() && pIsRunning != nullptr;
}

bool HTaskHandle::IsRunning() const
{
	if (IsValid() == false)
	{
		return false;
	}

	return (*pIsRunning) == true;
}

bool HTaskHandle::IsWaiting() const
{
	if (IsValid() == false)
	{
		return false;
	}

	return (*pIsRunning) == false;
}

void HTaskHandle::Wait()
{
	while (IsRunning()) { }
}