#pragma once
#include "JGCore.h"
#include <chrono>


namespace JG
{
	using jg_clock = std::chrono::steady_clock;

	enum class ETimeStepType
	{
		Hour,
		Minute,
		Second,
		MiliSecond
	};

	class Timer;
	class TimerHandle;
	class TimerManager : public GlobalSingleton<TimerManager>
	{
		static const u64 TIMER_NULL_ID = -1;
		friend class Application;
		friend TimerHandle;
		friend Timer;
	private:
		u64 mIDOffset = 0;
		std::mutex mMutex;
		Queue<u64> mIDQueue;
		Dictionary<u64, SharedPtr<Timer>> mTimers;
	public:
		void Update();
	private:
		SharedPtr<Timer> CreateTimer();
		void DestroyTimer(u64 timerID);
	};
	class Timer
	{
		friend TimerManager;
	private:
		u64 mID = TimerManager::TIMER_NULL_ID;
		jg_clock::time_point mTimePoint;
		f32 mTotalTime = 0.0f;
		f32 mDeltaTime = 0.0f;
		f32 mTimeElapsed = 0.0f;
		u32 mFPS = 0;
		u32 mFrameCount = 0;
		bool mIsRun = false;
		bool mIsFirst = true;
	private:
		void Tick();
	public:
		void Start();
		void Stop();
		f32 GetTotalTime(ETimeStepType stepType = ETimeStepType::Second) const;
		f32 GetTick(ETimeStepType stepType = ETimeStepType::Second) const;
		u64 GetFPS() const;
		bool IsValid() const;
	public:
		static SharedPtr<Timer> Create();
	};
}