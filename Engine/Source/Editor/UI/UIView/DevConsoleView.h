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
	class DevConsoleView : public UIView
	{
		JGCLASS
	private:
		enum class ELogFilter
		{
			None = 0,
			Info  = 0x001,
			Trace = 0x002,
			Warn  = 0x004,
			Error = 0x008,
		};
		enum class ELogLevel
		{
			Info,
			Trace,
			Warn,
			Error
		};
		struct LogFilterInfo
		{
			ELogFilter Filter;
			String	   ShowName;
			bool       IsActive;
		};
		struct LogInfo
		{
			ELogFilter Filter = ELogFilter::None;
			ELogLevel  LogLevel;
			String Message;
			u64  ThreadID;
		};
	private:
		static constexpr  f32 smTopHeight = 32.0f;
		static constexpr  f32 smBottomHeight = 32.0f;
		bool mOpenGUI = true;
		bool mAutoScroll = true;

		SharedPtr<ScheduleHandle> mReadScheduleHandle;
		Dictionary<ELogFilter, LogFilterInfo> mLogFilterInfoDic;

		u32 mStartLogFileOffset = 0;
		u32 mCurrentLogLine = 0;
		u32 mReadMaxLogLine = 10;
		u32 mMaxLogCount	= 9999;
		u32 mMainThreadID   = 0;


		List<LogInfo> mLogs;
		List<LogInfo> mPendingLogs;
		std::ifstream mLogFileStream;

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
		void OpenLogFile();
		void CloseLogFile();
		void OnGUI_Top();
		void OnGUI_Log();
		void OnGUI_Bottom();
		void UpdateLog();
		void ReadLog_Async();
		void PushLogInfo_Async(const String& msg);
	};
}


