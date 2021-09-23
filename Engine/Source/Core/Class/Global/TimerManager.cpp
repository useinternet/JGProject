#include "pch.h"
#include "TimerManager.h"

using namespace std::chrono;
using jg_clock = std::chrono::steady_clock;
namespace JG
{




    void TimerManager::Update()
    {
        static i32 delayFameCount = 0;
        if (delayFameCount < 5) delayFameCount++;

        for (auto& _pair : mTimers)
        {
            _pair.second->Tick();
        }
    }

    SharedPtr<Timer> TimerManager::CreateTimer()
    {
        std::lock_guard<std::mutex> lock(mMutex);
        u64 ID = 0;
        if (mIDQueue.empty())
        {
            ID = mIDOffset++;
        }
        else
        {
            ID = mIDQueue.front(); mIDQueue.pop();
        }
        
        mTimers[ID] = CreateSharedPtr<Timer>();
        mTimers[ID]->mID = ID;

        return mTimers[ID];
    }
    void TimerManager::DestroyTimer(u64 timerID)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        auto iter = mTimers.find(timerID);
        if (iter != mTimers.end())
        {
            mTimers.erase(timerID);
            mIDQueue.push(timerID);
        }
    }
    

    void Timer::Tick()
    {
        if (mIsFirst)
        {
            mTimePoint = jg_clock::now();
            mIsFirst = false;
            return;
        }
        if (mIsRun == false)
        {
            mDeltaTime  = 0.0f;
            mFrameCount = 0;
            mTimePoint  = jg_clock::now();
            return;
        }
        duration<float> sec = jg_clock::now() - mTimePoint;
        mDeltaTime = sec.count();
        if (mDeltaTime >= 0.0f)
        {
            mTotalTime += mDeltaTime;

            ++mFrameCount;
            mTimeElapsed += mDeltaTime;
            if (mTimeElapsed >= 1.0f)
            {
                mFPS = mFrameCount;
                mFrameCount = 0;
                mTimeElapsed = 0.0f;
            }
        }
        mTimePoint = jg_clock::now();
    }

    void Timer::Start()
    {
        mIsRun = true;
    }
    void Timer::Stop()
    {
        mIsRun = false;
    }
    f32 Timer::GetTotalTime(ETimeStepType stepType) const
    {
        return mTotalTime;
    }
    f32 Timer::GetTick(ETimeStepType stepType) const
    {
        return mDeltaTime;
    }
    u64 Timer::GetFPS() const
    {
        return mFPS;
    }
    bool Timer::IsValid() const
    {
        return mID != TimerManager::TIMER_NULL_ID;
    }
    SharedPtr<Timer> Timer::Create()
    {
        if (TimerManager::IsValid())
        {
            return TimerManager::GetInstance().CreateTimer();
        }
        return nullptr;
    }
}