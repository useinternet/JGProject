#include "pch.h"
#include "TaskController.h"

namespace JG
{
	void TaskController::AddTask(i32 _switch, const std::function<void()>& taskFunction)
	{
		Task task;
		task.Switch = _switch;
		task.Function = taskFunction;
		mTaskList.push_back(task);

	}
	void TaskController::SwitchOn(i32 _switch)
	{
		mTaskSwitch |= _switch;
	}
	void TaskController::SwitchOff(i32 _switch)
	{
		mTaskSwitch &= ~_switch;
	}
	void TaskController::Update()
	{
		for (auto& task : mTaskList)
		{
			if (task.Switch & mTaskSwitch)
			{
				if (task.Function)
				{
					task.Function();
				}
			}
		}
	}
}