#pragma once
#include "UI/UIView.h"


namespace JG
{
	/// <summary>
	/// 기능 개발 리스트
	/// 1. 로그 기능
	///		A. 로그 검색 기능
	///		B. 쓰레드 식별 기능
	///		C. 
	/// 2. 치트 기능,
	/// 
	/// </summary>
	
	class CommandPrompt;
	class DevConsoleView : public UIView
	{
		JGCLASS
	private:
		enum class ELogLevel
		{
			Info,
			Trace,
			Warn,
			Error
		};
		struct LogFilterInfo
		{
			ELogLevel  LogLevel;
			String	   ShowName;
			bool       IsActive;
		};
		struct LogInfo
		{
			ELogLevel  LogLevel;
			String Message;
			u64  ThreadID;
		};
	private:
		static constexpr  f32 smTopHeight = 60.0f;
		static constexpr  f32 smBottomHeight = 25.0f;
		bool mAutoScroll = true;

		SharedPtr<ScheduleHandle> mReadScheduleHandle;
		Dictionary<ELogLevel, LogFilterInfo> mLogFilterInfoDic;

		u32 mCurrentLogLine = 0;
		u32 mReadMaxLogLine = 10;
		u32 mMaxLogCount	= 9999;
		u32 mMainThreadID   = 0;


		List<LogInfo> mLogs;
		List<LogInfo> mPendingLogs;

		String mSearchStr;
		String mCmdStr;

		UniquePtr<CommandPrompt> mCommandPrompt;

		class DevConsoleSink;
		SharedPtr<DevConsoleSink> mSink;
	public:
		DevConsoleView();
		virtual ~DevConsoleView();
	protected:
		virtual void OnEvent(IEvent& e) override;
	protected:
		virtual void Load()		  override;
		virtual void Initialize() override;
		virtual void OnGUI()	  override;
		virtual void Destroy()	  override;

		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	private:
		void OnGUI_Top();
		void OnGUI_Log();
		void OnGUI_LogText(const LogInfo& info);
		void OnGUI_Bottom();
		void UpdateLog();
		void ReadLog_Async();
		void PushLogInfo_Async(const String& msg);
		void CommandExecution(const String& command);
	private:
		class DevConsoleSink : public spdlog::sinks::base_sink<std::mutex>
		{
			Queue<String> mLogMessageQueue;
		protected:
			void sink_it_(const spdlog::details::log_msg& msg) override
			{
				spdlog::memory_buf_t formatted;
				spdlog::sinks::base_sink<std::mutex>::formatter_->format(msg, formatted);
				mLogMessageQueue.push(fmt::to_string(formatted));
			}

			void flush_() override {}
		public:
			std::mutex& GetMutex() {
				return mutex_;
			}
			Queue<String>& GetLogMessageQueue() {
				return mLogMessageQueue;
			}
		};
	};
}


