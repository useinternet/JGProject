#pragma once
#include "JGCore.h"


namespace JG
{
	class TaskController
	{
		struct Task
		{
			i32 Switch = 0;
			std::function<void()> Function;
		};
	private:
		i32 mTaskSwitch = 0;
		List<Task> mTaskList;
	public:
		void AddTask(i32 _switch, const std::function<void()>& taskFunction);
		void SwitchOn(i32 _switch);
		void SwitchOff(i32 _switch);
	public:
		void Update();
	};
}