#pragma once
#include "JGCore.h"
#include <thread>
#include <mutex>

namespace JG
{
#define SCHEDULE_BIND_FN(func) std::bind(func, this)


	class SchedulePriority
	{
	public:
		enum 
		{
			// System 
			BeginSystem = -2000000,

			Graphics_BeginFrame		  = BeginSystem + 1,
			Animation_BeginFrame      = Graphics_BeginFrame,


			Graphics_PushSceneObject  = BeginSystem + 2,
			Graphics_Rendering        = BeginSystem + 3,

			AssetDataBase_Update      = BeginSystem + 4,
			// Layer
			ImGuiSystemLayer		 = -900000,
			GameLayer				 = -800000,
			UISystemLayer            = -700000,
			MainMenuLayer			 = -600000,
			//
			Default = 0,
			// GameLogic
			DestroyGameClass = 100000,
			UpdateGameClass  = 200000,

			// System
			OnGUI			       = 1000000,
			Graphics_RenderFinish  = 2000000,
			Animation_EndFrame     = Graphics_RenderFinish,
			Graphics_EndFrame	   = 2000001,
			

			DestroyObject          = 2000002,
			Graphics_DestroyObject = 2000003,
			EndSystem = 10000000,
		};
	};
	enum class EScheduleState
	{
		Wait,
		Run,
		Compelete,
	};
	enum class EScheduleType
	{
		SyncByTick,
		SyncByFrame,
		Async
	};
	enum class EScheduleResult
	{
		Continue,
		Break,
	};

	


	class Timer;
	class Application;
	class Scheduler;

	class ScheduleHandle
	{
		friend Scheduler;

		u64 mID = 0;
		mutable std::mutex mMutex;
		EScheduleState mState;
		EScheduleType  mType;
		SharedPtr<IJGObject> mUserData = nullptr;
	private:
		ScheduleHandle(const ScheduleHandle& copy) = delete;
		ScheduleHandle& operator=(const ScheduleHandle& copy) = delete;
	public:
		ScheduleHandle() = default;
		~ScheduleHandle();
	public:
		u64 GetID() const {
			return mID;
		}
		EScheduleState GetState() const {
			std::lock_guard<std::mutex> lock(mMutex);
			return mState;
		}
		void SetState(EScheduleState state)
		{
			std::lock_guard<std::mutex> lock(mMutex);
			mState = state;
		}
		EScheduleType GetType() const {
			return mType;
		}
		bool IsValid() const;
		void Reset();
		SharedPtr<IJGObject> GetUserData() const {
			return mUserData;
		}
	};





	using SyncTaskFunction  = std::function<EScheduleResult()>;
	using SyncTaskUserDataFunction = std::function<EScheduleResult(SharedPtr<IJGObject> userData)>;
	using AsyncTaskFunction = std::function<void()>;
	using AsyncTaskUserDataFunction = std::function<void(SharedPtr<IJGObject> userData)>;
	// Worker 를 생성하여 스케쥴 작성
	class Scheduler : public IGlobalSingleton<Scheduler>
	{
		friend class Application;
		friend ScheduleHandle;
		static const u64 SCHEDULE_NULL_ID = -1;
	private:
		class SyncTask
		{
		public:
			SharedPtr<ScheduleHandle> Handle;
			u64 ID = SCHEDULE_NULL_ID;
			i32 Repeat = 0;
			i32 Priority = 0;
			SyncTaskUserDataFunction UserDataFunction;
			SyncTaskFunction Function;
			i32 CallCount = 0;
		public:
			virtual ~SyncTask() = default;
		public:
			virtual EScheduleType GetScheduleType() const = 0;
		};
		class SyncTaskByTick : public SyncTask
		{
		public:
			f32 Delay     = 0.0f;
			f32 TickCycle = 0.0f;
			f32 Tick = 0.0f;
		public:
			virtual ~SyncTaskByTick() = default;
			virtual EScheduleType GetScheduleType() const {
				return EScheduleType::SyncByTick;
			}
		public:

		};
		class SyncTaskByFrame : public SyncTask
		{
		public:
			i32 Delay = 0;
			i32 FrameCycle = 0;
			i32 Frame = 0;
		public:
			virtual ~SyncTaskByFrame() = default;
			virtual EScheduleType GetScheduleType() const {
				return EScheduleType::SyncByFrame;
			}
		};
		struct AsyncTask
		{
			SharedPtr<ScheduleHandle> Handle;
			AsyncTaskUserDataFunction UserDataFunction;
			AsyncTaskFunction Function;
			void SetState(EScheduleState State) {
				Handle->SetState(State);
			}
		};
	private:


		u64 mIDOffset = 1;
		Queue<u64> mIDQueue;
		std::thread::id mMainThreadID;
		SharedPtr<Timer> mScheduleTimer;
		//
		Dictionary<u64, SharedPtr<SyncTask>>  mSyncTaskPool;
		SortedDictionary<i32, List<WeakPtr<SyncTask>>>  mSortedSyncTasks;
		Queue<WeakPtr<SyncTask>> mReservedSyncTasks;
		
		// Thread 관련
		Queue<SharedPtr<AsyncTask>> mAsyncTaskQueue;
		List<std::thread>			mThreads;
		i32		   mMaxThreadCount		  = 0;
		const u64  mMaxRemoveWeakPtrCount = 100;
		std::mutex mMutex;
		std::mutex mTaskMutex;

		std::condition_variable mRunAsyncTaskConVar;
		std::atomic_bool mIsRunAsyncTaskAll = true;
		bool mIsRunSyncTaskAll  = false;



		mutable Dictionary<std::thread::id, u64> mThreadMappingIDs;
		mutable std::mutex mMappingIDMutex;
		mutable u64 mThreadIndex = 0;
	public:
		Scheduler();
		virtual ~Scheduler();
	public:
		SharedPtr<ScheduleHandle> Schedule(f32 delay, f32 tickCycle, i32 repeat, i32 priority, const SyncTaskFunction& task);
		SharedPtr<ScheduleHandle> ScheduleOnce(f32 delay, i32 priority, const SyncTaskFunction& task);
		SharedPtr<ScheduleHandle> ScheduleByFrame(i32 delayFrame, i32 frameCycle, i32 repeat, i32 priority, const SyncTaskFunction& task);
		SharedPtr<ScheduleHandle> ScheduleOnceByFrame(i32 delayFrame, i32 priority, const SyncTaskFunction& task);


		SharedPtr<ScheduleHandle> Schedule(f32 delay, f32 tickCycle, i32 repeat, i32 priority, const SyncTaskUserDataFunction& task, SharedPtr<IJGObject> userData);
		SharedPtr<ScheduleHandle> ScheduleOnce(f32 delay, i32 priority, const SyncTaskUserDataFunction& task, SharedPtr<IJGObject> userData);
		SharedPtr<ScheduleHandle> ScheduleByFrame(i32 delayFrame, i32 frameCycle, i32 repeat, i32 priority, const SyncTaskUserDataFunction& task, SharedPtr<IJGObject> userData);
		SharedPtr<ScheduleHandle> ScheduleOnceByFrame(i32 delayFrame, i32 priority, const SyncTaskUserDataFunction& task, SharedPtr<IJGObject> userData);



		
		SharedPtr<ScheduleHandle> ScheduleAsync(const AsyncTaskFunction& task);
		SharedPtr<ScheduleHandle> ScheduleAsync(const AsyncTaskUserDataFunction& task, SharedPtr<IJGObject> userData = nullptr);

		void FlushAsyncTask(bool isRestart = true);
		const Timer* GetScheduleTimer() const;
	public:
		u64 GetThreadMappingID(std::thread::id threadID) const;
		u64 GetThisThreadMappingID() const;
		u64 GetThreadCount() const;
	private:
		void Update();
		void Update(SyncTaskByTick* task);
		void Update(SyncTaskByFrame* task);
		void ResultProcess(SyncTaskByTick* task, EScheduleResult result);
		void ResultProcess(SyncTaskByFrame* task, EScheduleResult result);
		void RemoveSchedule(const ScheduleHandle& handle);
		u64 ReceiveScheduleID();
	};


}