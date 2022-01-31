#include "pch.h"
#include "DevConsoleView.h"
#include "UI/UIManager.h"

namespace JG
{
	DevConsoleView::DevConsoleView()
	{
		UIManager::GetInstance().RegisterMainMenuItem("Windows/DevlopConsole", 0, [&]()
		{
			Open();
		}, nullptr);

		mMainThreadID = StringHelper::ToInt(StringHelper::ToString(std::this_thread::get_id()));
	}
	DevConsoleView::~DevConsoleView()
	{

	}
	void DevConsoleView::OnEvent(IEvent& e)
	{


	}
	void DevConsoleView::Load()
	{
		LogFilterInfo info;
		info.IsActive = true;
		info.Filter = ELogFilter::Info;
		info.ShowName = "Info";
		mLogFilterInfoDic[ELogFilter::Info] = info;


		info.Filter = ELogFilter::Trace;
		info.ShowName = "Trace";
		mLogFilterInfoDic[ELogFilter::Trace] = info;

		info.Filter = ELogFilter::Warn;
		info.ShowName = "Warn";
		mLogFilterInfoDic[ELogFilter::Warn] = info;

		info.Filter = ELogFilter::Error;
		info.ShowName = "Error";
		mLogFilterInfoDic[ELogFilter::Error] = info;
	}
	void DevConsoleView::Initialize()
	{
	}
	void DevConsoleView::OnGUI()
	{
		ImGui::Begin("DevConsole", &mOpenGUI, ImGuiWindowFlags_NoScrollbar);
		UpdateLog();
		OnGUI_Top();
		OnGUI_Log();
		OnGUI_Bottom();

		static int s = 0;
		if (ImGui::IsKeyPressed((int)EKeyCode::E))
		{
			JG_LOG_TRACE("{0} TEST", s);
			s++;

		}
		ImGui::End();
		if (mOpenGUI == false)
		{
			mOpenGUI = true;
			Close();
		}
	}
	void DevConsoleView::Destroy()
	{

	}
	void DevConsoleView::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		UIView::MakeJson(jsonData);
	}
	void DevConsoleView::LoadJson(SharedPtr<JsonData> jsonData)
	{
		UIView::LoadJson(jsonData);
	}
	void DevConsoleView::OpenLogFile()
	{
		if (mLogFileStream.is_open() == false)
		{
			if (fs::exists(Log::GetLogFile()) == false)
			{
				FileHelper::WriteAllText(Log::GetLogFile(), "");
			}
			mLogFileStream.open(Log::GetLogFile());
			mLogFileStream.seekg(mStartLogFileOffset, std::ios::beg);
		}
	}
	void DevConsoleView::CloseLogFile()
	{
		if (mLogFileStream.is_open())
		{
			mLogFileStream.close();
		}

	}
	void DevConsoleView::OnGUI_Top()
	{
		ImGui::BeginChild("DevConsoleView/Top", ImVec2(0, smTopHeight));
		ImGui::Spacing();

		
		ImGui::SetNextItemWidth(100.0f);
		if (ImGui::BeginCombo("##Log_Filter","Filter"))
		{
			ImGui::Columns(2, 0, false);
	
			f32 checkbox_width = 32.0f;
			ImGui::SetColumnWidth(0, 100.0f - checkbox_width);
			ImGui::SetColumnWidth(1, checkbox_width);


			for (auto& _pair : mLogFilterInfoDic)
			{
				LogFilterInfo& info = _pair.second;
				ImGui::AlignTextToFramePadding();
				ImGui::Text(info.ShowName.c_str());
				ImGui::NextColumn();
				info.IsActive = ImGui::CheckBox(&info, info.IsActive);
				ImGui::NextColumn();
			}
			ImGui::Columns(1);		
			ImGui::EndCombo();
		}
		ImGui::SameLine();
		mAutoScroll = ImGui::CheckBox(&mAutoScroll, mAutoScroll);
		ImGui::AlignTextToFramePadding();

		ImGui::SameLine(ImGui::GetContentRegionMax().x - 60.0f);
		if (ImGui::Button("Clear"))
		{
			// Clear
		}

		ImGui::EndChild();
		ImGui::Separator();
	}
	void DevConsoleView::OnGUI_Log()
	{
		float height = ImGui::GetContentRegionAvail().y - (smBottomHeight + smTopHeight) - ImGui::GetStyle().ItemSpacing.y * 4;
		ImGui::BeginChild("DevConsoleView/Log", ImVec2(0, height), true);

		u64 logCnt = mLogs.size();
		if (logCnt >= mMaxLogCount)
		{
			for (u64 i = 0; i < mMaxLogCount; ++i)
			{
				u64 line = (mCurrentLogLine + i) % mMaxLogCount;
				ImGui::TextWrapped((mLogs[line].Message).c_str());
			}
		}
		else
		{
			for (auto& loginfo : mLogs)
			{
				
				ImGui::TextWrapped((loginfo.Message).c_str());
	
			}
		}
		if (mAutoScroll)
		{
			ImGui::SetScrollHereY(1.0f);
		}
		ImGui::EndChild();
		ImGui::Separator();
	}
	void DevConsoleView::OnGUI_Bottom()
	{
		String out;
		ImGui::BeginChild("DevConsoleView/Bottom", ImVec2(0, smBottomHeight));
		ImGui::Spacing();
		ImGui::InputText(this, "(null)", out);
		ImGui::EndChild();
		ImGui::Separator();
	}
	void DevConsoleView::UpdateLog()
	{

		if (mReadScheduleHandle == nullptr || mReadScheduleHandle->GetState() == EScheduleState::Compelete)
		{
			// 데이터 전달
			if (mPendingLogs.empty() == false)
			{
				if (mLogs.size() + mPendingLogs.size() >= mMaxLogCount)
				{
					u64 addLine = 0;
					for (u64 i = 0; i < mPendingLogs.size(); ++i)
					{
						u64 line = (i + mCurrentLogLine) % mMaxLogCount;

						if (mLogs.size() <= line)
						{
							mLogs.push_back(mPendingLogs[i]);
						}
						else
						{
							addLine++;
							mLogs[line] = mPendingLogs[i];
						}

					}
					mCurrentLogLine += addLine;
			
				}
				else
				{
					mLogs.insert(mLogs.end(), mPendingLogs.begin(), mPendingLogs.end());
				}

			}
			
			mPendingLogs.clear();
			mReadScheduleHandle = Scheduler::GetInstance().ScheduleAsync([&]()
			{
				OpenLogFile();
				ReadLog_Async();
				CloseLogFile();
			});
		}
	}
	void DevConsoleView::ReadLog_Async()
	{
		if (mLogFileStream.is_open())
		{
			u32 readLineCount = 0;
			while (mLogFileStream.eof() == false)
			{
			
				String log;
				std::getline(mLogFileStream, log);

				String token = "[_]";

				if (StringHelper::StartWidth(log, token) == false || StringHelper::EndWidth(log, token) == false)
				{
					break;
				}
				mStartLogFileOffset = mLogFileStream.tellg();


				log = StringHelper::ReplaceAll(log, "[_]", "");
				PushLogInfo_Async(log);
				++readLineCount;
				if (readLineCount > mReadMaxLogLine)
				{
					break;
				}
			}
		

		}
	}
	void DevConsoleView::PushLogInfo_Async(const String& msg)
	{
		if (msg.length() <= 0)
		{
			return;
		}
		LogInfo info;
		info.Message = msg;
		if (msg.find("[trace]") != String::npos)
		{
			info.Filter = (ELogFilter)((i32)info.Filter | (i32)ELogFilter::Trace);
			info.LogLevel = ELogLevel::Trace;
			info.Message = StringHelper::ReplaceAll(info.Message, "[trace]", "");
		}
		else if (msg.find("[info]") != String::npos)
		{
			info.Filter = (ELogFilter)((i32)info.Filter | (i32)ELogFilter::Info);
			info.LogLevel = ELogLevel::Info;
			info.Message = StringHelper::ReplaceAll(info.Message, "[info]", "");
		}
		else if (msg.find("[warning]") != String::npos)
		{
			info.Filter = (ELogFilter)((i32)info.Filter | (i32)ELogFilter::Warn);
			info.LogLevel = ELogLevel::Warn;
			info.Message = StringHelper::ReplaceAll(info.Message, "[warning]", "");
		}
		else if (msg.find("[error]") != String::npos)
		{
			info.Filter = (ELogFilter)((i32)info.Filter | (i32)ELogFilter::Error);
			info.LogLevel = ELogLevel::Error;
			info.Message = StringHelper::ReplaceAll(info.Message, "[error]", "");
		}
	


		String threadStr;
		u64 startPos = msg.find("[:");
		if (startPos != String::npos)
		{
			startPos += 2;
			u64 endPos = msg.find(":]", startPos);

			threadStr = msg.substr(startPos, endPos - startPos);

			info.Message = StringHelper::ReplaceAll(info.Message, "[:" + threadStr + ":]", "");


			u64 threadID = StringHelper::ToInt(threadStr);
			if (threadID != mMainThreadID)
			{
				info.Message = "[SubThread]" + info.Message;
			}
		}
		mPendingLogs.push_back(info);
	}
}