#include "pch.h"
#include "Scheduler.h"

namespace JG
{
    ScheduleHandle::~ScheduleHandle()
    {
        Reset();
    }
    bool ScheduleHandle::IsValid() const
    {
        return mID != Scheduler::SCHEDULE_NULL_ID;
    }
    void ScheduleHandle::Reset()
    {
        if (Scheduler::IsValid() && mID != Scheduler::SCHEDULE_NULL_ID)
        {
            Scheduler::GetInstance().RemoveSchedule(*this);
            mID = Scheduler::SCHEDULE_NULL_ID;
            if (UserData != nullptr)
            {
                free(UserData);
                UserData = nullptr;
            }
        }
    }

    Scheduler::Scheduler()
    {
        mMainThreadID = std::this_thread::get_id();
        mMaxThreadCount = std::thread::hardware_concurrency();

        JG_CORE_INFO("Scheduler Prepared Thread Count :  {0} ", mMaxThreadCount);

        mThreads.resize(mMaxThreadCount);
        for (i32 i = 0; i < mMaxThreadCount; ++i)
        {
            mThreads[i] = std::thread([&]()
            {
                while (mIsRunAsyncTaskAll)
                {
                    std::unique_lock<std::mutex> lock(mMutex);
                    mRunAsyncTaskConVar.wait(lock, [&]()->bool
                    {
                        return (mAsyncTaskQueue.empty() == false || mIsRunAsyncTaskAll == false);
                    });

                    SharedPtr<AsyncTask> task = nullptr;
                    if (mIsRunAsyncTaskAll)
                    {
                        task = mAsyncTaskQueue.front(); mAsyncTaskQueue.pop();
                    }
                    lock.unlock();

                    if (task != nullptr)
                    {
                        // Task ½ÇÇà
                        task->SetState(EScheduleState::Run);
                        task->Function(task->Handle->UserData);
                        task->SetState(EScheduleState::Compelete);
                    }

                }
            });
            
        }
        mScheduleTimer = Timer::Create();
        mScheduleTimer->Start();
    }
    Scheduler::~Scheduler()
    {
        FlushAsyncTask(false);
        for (auto& _pair : mSyncTaskPool)
        {
            switch (_pair.second->GetScheduleType())
            {
            case EScheduleType::SyncByFrame:
            {
                auto frameTask = static_cast<SyncTaskByFrame*>(_pair.second.get());
                frameTask->ID = SCHEDULE_NULL_ID;
                frameTask->Handle->mID = SCHEDULE_NULL_ID;
            }
  
                break;
            case EScheduleType::SyncByTick:
            {
                auto tickTask = static_cast<SyncTaskByTick*>(_pair.second.get());
                tickTask->ID = SCHEDULE_NULL_ID;
                tickTask->Handle->mID = SCHEDULE_NULL_ID;
            }
                break;
            }
        }
        mSortedSyncTasks.clear();
        mSyncTaskPool.clear();
        mThreads.clear();
    }
    SharedPtr<ScheduleHandle> Scheduler::Schedule(f32 delay, f32 tickCycle, i32 repeat, i32 priority, const SyncTaskFunction& task)
    {
        auto SyncTask = CreateSharedPtr<SyncTaskByTick>();
        SyncTask->Delay = delay;
        SyncTask->TickCycle = tickCycle;
        SyncTask->Repeat = repeat;
        SyncTask->Priority = priority;
        SyncTask->Function = task;

        auto handle = CreateSharedPtr<ScheduleHandle>();
        handle->mState = EScheduleState::Wait;
        handle->mType  = EScheduleType::SyncByTick;
        SyncTask->Handle = handle;




        std::lock_guard<std::mutex> lock(mTaskMutex);
        u64 ID = ReceiveScheduleID();
        SyncTask->ID = ID;
        handle->mID  = ID;
        mSyncTaskPool.emplace(ID, SyncTask);
        if (mIsRunSyncTaskAll || mMainThreadID != std::this_thread::get_id())
        {
            mReservedSyncTasks.push(SyncTask);
        }
        else
        {
            mSortedSyncTasks[SyncTask->Priority].push_back(SyncTask);
        }
        return handle;
    }
    SharedPtr<ScheduleHandle> Scheduler::ScheduleOnce(f32 delay, i32 priority, const SyncTaskFunction& task)
    {
        return Schedule(delay, 0.0f, 1, priority, task);
    }
    SharedPtr<ScheduleHandle> Scheduler::ScheduleByFrame(i32 delayFrame, i32 frameCycle, i32 repeat, i32 priority, const SyncTaskFunction& task)
    {
        auto SyncTask = CreateSharedPtr<SyncTaskByFrame>();
        SyncTask->Delay = delayFrame;
        SyncTask->FrameCycle = frameCycle;
        SyncTask->Repeat = repeat;
        SyncTask->Priority = priority;
        SyncTask->Function = task;
       
        auto handle = CreateSharedPtr<ScheduleHandle>();
        handle->mState = EScheduleState::Wait;
        handle->mType = EScheduleType::SyncByFrame;
        SyncTask->Handle = handle;


        std::lock_guard<std::mutex> lock(mTaskMutex);
        u64 ID = ReceiveScheduleID();
        SyncTask->ID = ID;
        handle->mID = ID;
        mSyncTaskPool.emplace(ID, SyncTask);
        if (mIsRunSyncTaskAll || mMainThreadID != std::this_thread::get_id())
        {
            mReservedSyncTasks.push(SyncTask);
        }
        else
        {
            mSortedSyncTasks[SyncTask->Priority].push_back(SyncTask);
        }
        return handle;
    }
    SharedPtr<ScheduleHandle> Scheduler::ScheduleOnceByFrame(i32 delayFrame, i32 priority, const SyncTaskFunction& task)
    {
        return ScheduleByFrame(delayFrame, 0, 1, priority, task);
    }
    SharedPtr<ScheduleHandle> Scheduler::ScheduleAsync(const AsyncTaskFunction& task, void* userData, u64 dataSize)
    {
        auto asyncTask = CreateSharedPtr<AsyncTask>();
        auto handle    = CreateSharedPtr<ScheduleHandle>();
        {

            handle->mID = 0;
            handle->mState = EScheduleState::Wait;
            handle->mType  = EScheduleType::Async;
            if (userData != nullptr)
            {
                handle->UserData = malloc(dataSize);
                memcpy(handle->UserData, userData, dataSize);
            }
            asyncTask->Handle = handle;
            asyncTask->Function = task;

            std::lock_guard<std::mutex> lock(mMutex);
            mAsyncTaskQueue.push(asyncTask);
        }
        mRunAsyncTaskConVar.notify_one();
        return handle;
    }
    void Scheduler::FlushAsyncTask(bool isRestart)
    {
        if (isRestart == false)
        {
            mIsRunAsyncTaskAll = false;
        }

        {
            std::lock_guard<std::mutex> lock(mMutex);
            mRunAsyncTaskConVar.notify_all();
        }
        while (mAsyncTaskQueue.empty() == false) {}
        if (isRestart == false)
        {
            for (auto& _thread : mThreads)
            {
                if (_thread.joinable())
                {
                    _thread.join();
                }
            }
        }
    }
    const Timer* Scheduler::GetScheduleTimer() const
    {
        return mScheduleTimer.get();
    }
    u64 Scheduler::GetThreadMappingID(std::thread::id threadID) const
    {
        std::lock_guard<std::mutex> lock(mMappingIDMutex);
        if (mThreadMappingIDs.find(threadID) == mThreadMappingIDs.end())
        {
            mThreadMappingIDs[threadID] = mThreadIndex;
            mThreadIndex++;
        }
        return mThreadMappingIDs[threadID];
    }
    u64 Scheduler::GetThisThreadMappingID() const
    {
        return GetThreadMappingID(std::this_thread::get_id());
    }
    void Scheduler::Update()
    {
        mIsRunSyncTaskAll = true;

        for (auto& _pair : mSortedSyncTasks)
        {
            auto& taskList = _pair.second;
            u64 cnt = _pair.second.size();

            for (u64 i = 0; i < cnt; ++i)
            {
                auto task = taskList[i].lock();

                if (task)
                {
                    switch (task->GetScheduleType())
                    {
                    case EScheduleType::SyncByFrame:
                    {
                        auto frameTask = static_cast<SyncTaskByFrame*>(task.get());
                        Update(frameTask);
                    }
                    break;
                    case EScheduleType::SyncByTick:
                    {
                        auto tickTask = static_cast<SyncTaskByTick*>(task.get());
                        Update(tickTask);
                    }
                    break;
                    }
                }
            }
        }


        std::lock_guard<std::mutex> lock(mTaskMutex);
        while (mReservedSyncTasks.empty() == false)
        {
            auto w_task = mReservedSyncTasks.front(); mReservedSyncTasks.pop();
            auto task = w_task.lock();
            if (task != nullptr)
            {
                mSortedSyncTasks[task->Priority].push_back(task);
            }
        }
        mIsRunSyncTaskAll = false;
    }
    void Scheduler::Update(SyncTaskByTick* task)
    {
        if (task->Handle->GetState() == EScheduleState::Compelete)
        {
            task->Handle->Reset();
            return;
        }

        f32 tick = mScheduleTimer->GetTick();
        task->Tick += tick;

        if (task->Handle->GetState() == EScheduleState::Wait)
        {
            if (task->Delay <= task->Tick)
            {
                task->Handle->mState = EScheduleState::Run;
                task->Tick = 0.0f;
            }
        }


        if (task->Handle->GetState() == EScheduleState::Run)
        {
            if (task->TickCycle <= task->Tick)
            {
                task->Tick = 0.0f;
                task->CallCount += 1;

                if (task->Function)
                {
                    auto result = task->Function();
                    ResultProcess(task, result);
                }
       

                if (task->Repeat != -1 && task->CallCount >= task->Repeat)
                {
                    task->Handle->mState = EScheduleState::Compelete;
                }
            }
        }
    }
    void Scheduler::Update(SyncTaskByFrame* task)
    {
        if (task->Handle->GetState() == EScheduleState::Compelete)
        {
            task->Handle->Reset();
            return;
        }
        task->Frame++;

        if (task->Handle->GetState() == EScheduleState::Wait)
        {
            if (task->Delay <= task->Frame)
            {
                task->Handle->mState = EScheduleState::Run;
                task->Frame = 0;
            }
        }

        if (task->Handle->GetState() == EScheduleState::Run)
        {
            if (task->FrameCycle <= task->Frame)
            {
                task->Frame = 0;
                task->CallCount += 1;

                if (task->Function)
                {
                    auto result = task->Function();
                    ResultProcess(task, result);
                }

                if (task->Repeat != -1 && task->CallCount >= task->Repeat)
                {
                    task->Handle->mState = EScheduleState::Compelete;
                }
            }
        }

    }
    void Scheduler::ResultProcess(SyncTaskByTick* task, EScheduleResult result)
    {
        switch (result)
        {
        case EScheduleResult::Continue:
            break;
        case EScheduleResult::Break:
            task->Handle->mState = EScheduleState::Compelete;
            break;
        }
    }
    void Scheduler::ResultProcess(SyncTaskByFrame* task, EScheduleResult result)
    {
        switch (result)
        {
        case EScheduleResult::Continue:
            break;
        case EScheduleResult::Break:
            task->Handle->mState = EScheduleState::Compelete;
            break;
        }
    }
    void Scheduler::RemoveSchedule(const ScheduleHandle& handle)
    {
        mSyncTaskPool.erase(handle.mID);
    }
    u64 Scheduler::ReceiveScheduleID()
    {
        if (mIDQueue.empty())
        {
            return mIDOffset++;
        }
        else
        {
            u64 ID = mIDQueue.front(); mIDQueue.pop();
            return ID;
        }
    }
}