#pragma once
#include "JGCore.h"
#include "Class/Global/Scheduler.h"

namespace JG
{
	class ScheduleHandle;
	class TaskController
	{

		struct Task
		{
			i32 ID = 0;
			std::function<void(SharedPtr<IJGObject>)> Ready;
			std::function<void(SharedPtr<IJGObject>)> Run;
			std::function<i32(SharedPtr<IJGObject>)>  Next;
			
			SharedPtr<ScheduleHandle> Handle;
		};
		Dictionary<i32, Task> mTaskPool;
		SharedPtr<IJGObject> mUserData;
		i32 mStartTaskID = -1;
		i32 mCurrTaskID  = -1;
		u64 mPriority = 0;

		std::atomic_bool mIsRun = false;
	public:
		~TaskController();
	public:
		void AddTask(u32 id, 
			const std::function<void(SharedPtr<IJGObject>)>& readyTask, 
			const std::function<void(SharedPtr<IJGObject>)>& runTask, 
			const std::function<i32(SharedPtr<IJGObject>)>& nextTask);
		void SetPriority(u64 priority);
		void SetStartID(i32 startTaskID);
		void SetUserData(SharedPtr<IJGObject> userData);
		bool IsRun() const;
		void Run();
		void Reset();
	private:
		bool IsExistTask(i32 taskID) const;
	};

}