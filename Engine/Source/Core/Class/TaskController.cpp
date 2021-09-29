#include "TaskController.h"
#include "TaskController.h"
#include "TaskController.h"
#include "TaskController.h"
#include "pch.h"
#include "TaskController.h"

namespace JG
{
	TaskController::~TaskController()
	{
		Reset();
	}
	void TaskController::AddTask(u32 id,
		const std::function<void(SharedPtr<IJGObject>)>& readyTask,
		const std::function<void(SharedPtr<IJGObject>)>& runTask,
		const std::function<i32(SharedPtr<IJGObject>)>& nextTask)
	{
		if (mIsRun == true)
		{
			assert("can't add task while run taskController");
		}



		Task task;
		task.ID = id;
		task.Ready = readyTask;
		task.Run   = runTask;
		task.Next  = nextTask;
		mTaskPool[id] = task;
	}

	void TaskController::SetPriority(u64 priority)
	{
		mPriority = priority;
	}

	void TaskController::SetStartID(i32 startTaskID)
	{
		mStartTaskID = startTaskID;
	}

	void TaskController::SetUserData(SharedPtr<IJGObject> userData)
	{
		if (mIsRun) return;
		mUserData = userData;
	}

	bool TaskController::IsRun() const
	{
		return mIsRun;
	}

	void TaskController::Run()
	{
		if (mIsRun == false)
		{
			mIsRun = true;
			if (IsExistTask(mStartTaskID) == false)
			{
				return;
			}
			mCurrTaskID = mStartTaskID;
			for (auto& task : mTaskPool)
			{
				if (task.second.Ready)
				{
					task.second.Ready(mUserData);
				}
			}

			Scheduler::GetInstance().ScheduleByFrame(0, 1, -1, mPriority,
				[&]()->EScheduleResult
			{
				if (mIsRun == false)   return EScheduleResult::Break;
				if (mCurrTaskID == -1)
				{
					mIsRun = false;
					return EScheduleResult::Break;
				}
				if (IsExistTask(mCurrTaskID) == false)
				{
					EScheduleResult::Break;
				}

				auto& task = mTaskPool[mCurrTaskID];
				if (task.Handle && task.Handle->GetState() == EScheduleState::Compelete)
				{
					if (task.Next)
					{
						mCurrTaskID = task.Next(mUserData);
					}
					else
					{
						mCurrTaskID = -1;
					}
				}
				// 핸들이 처음일 경우
				if (task.Handle == nullptr)
				{
					task.Handle = Scheduler::GetInstance().ScheduleAsync([&](SharedPtr<IJGObject> userData)
					{
						if (task.Run)
						{
							task.Run(userData);
						}
					}, mUserData);
				}
			});

		}
	}
	void TaskController::Reset()
	{
		mIsRun = false;
		mCurrTaskID = mStartTaskID;
		for (auto& task : mTaskPool)
		{
			if (task.second.Handle)
			{
				task.second.Handle->Reset();
				task.second.Handle = nullptr;
			}
		}
	}

	bool TaskController::IsExistTask(i32 taskID) const
	{
		return mTaskPool.find(taskID) != mTaskPool.end();
	}

}