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