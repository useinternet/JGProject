#include "pch.h"
#include "DevConsoleView.h"
#include "ExternalImpl/CommandPrompt.h"
#include "UI/UIManager.h"

namespace JG
{
	DevConsoleView::DevConsoleView()
	{
		mSink = CreateUniquePtr<DevConsoleSink>();
		mSink->set_pattern("%^[:%t:][%l]%v%$");
		Log::GetLogger()->sinks().push_back(mSink);
		UIManager::GetInstance().RegisterMainMenuItem("Windows/DevlopConsole", 0, [&]()
		{
			Open();
		}, nullptr);
		mCommandPrompt = CreateUniquePtr<CommandPrompt>();
		mMainThreadID = StringHelper::ToInt(StringHelper::ToString(std::this_thread::get_id()));

		SetTitleName("DevConsole");
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
		info.LogLevel = ELogLevel::Info;
		info.ShowName = "Info";
		mLogFilterInfoDic[ELogLevel::Info] = info;


		info.LogLevel = ELogLevel::Trace;
		info.ShowName = "Trace";
		mLogFilterInfoDic[ELogLevel::Trace] = info;

		info.LogLevel = ELogLevel::Warn;
		info.ShowName = "Warn";
		mLogFilterInfoDic[ELogLevel::Warn] = info;

		info.LogLevel = ELogLevel::Error;
		info.ShowName = "Error";
		mLogFilterInfoDic[ELogLevel::Error] = info;
	}
	void DevConsoleView::Initialize()
	{

	}
	void DevConsoleView::OnGUI()
	{
		UpdateLog();
		OnGUI_Top();
		OnGUI_Log();
		OnGUI_Bottom();
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
		ImGui::Text("Auto Scroll"); ImGui::SameLine();
		mAutoScroll = ImGui::CheckBox(&mAutoScroll, mAutoScroll);
		ImGui::AlignTextToFramePadding();

		ImGui::SameLine(ImGui::GetContentRegionMax().x - 60.0f);
		if (ImGui::Button("Clear"))
		{
			mLogs.clear();
		}
		ImGui::Separator();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Filter"); ImGui::SameLine();
		ImGui::InputText("DevConsoleView/SearchInputText", mSearchStr, mSearchStr);
	
		ImGui::EndChild();
		ImGui::Separator();
	}
	void DevConsoleView::OnGUI_Log()
	{
		float height = ImGui::GetContentRegionAvail().y - (smBottomHeight) - ImGui::GetStyle().ItemSpacing.y * 2;
		ImGui::BeginChild("DevConsoleView/Log", ImVec2(0, height), true);

		u64 logCnt = mLogs.size();
		if (logCnt >= mMaxLogCount)
		{
			for (u64 i = 0; i < mMaxLogCount; ++i)
			{
				u64 line = (mCurrentLogLine + i) % mMaxLogCount;
				OnGUI_LogText(mLogs[line]);
			}
		}
		else
		{
			for (auto& loginfo : mLogs)
			{
				OnGUI_LogText(loginfo);
	
			}
		}
		if (mAutoScroll)
		{
			ImGui::SetScrollHereY(1.0f);
		}
		ImGui::EndChild();

	}
	void DevConsoleView::OnGUI_LogText(const LogInfo& info)
	{
		bool isShow = mLogFilterInfoDic[info.LogLevel].IsActive;
		if (isShow == true && mSearchStr.length() > 0)
		{
			isShow = info.Message.find(mSearchStr) != String::npos;
		}
		if (isShow == false)
		{
			return;
		}
		
		ImVec4 texColor = ImVec4(1, 1, 1, 1);
		switch (info.LogLevel)
		{
		case ELogLevel::Info:
			texColor = ImVec4(0, 1, 0, 1);
			break;
		case ELogLevel::Warn:
			texColor = ImVec4(1, 1, 0, 1);
			break;
		case ELogLevel::Error:
			texColor = ImVec4(1, 0, 0, 1);
			break;
		}
		ImGui::PushStyleColor(ImGuiCol_Text, texColor);


		ImGui::TextWrapped(info.Message.c_str());

		ImGui::PopStyleColor();
	}
	void DevConsoleView::OnGUI_Bottom()
	{
		ImGui::BeginChild("DevConsoleView/Bottom", ImVec2(0, smBottomHeight));


		ImGui::AlignTextToFramePadding();
		ImGui::Text("Cmd   "); ImGui::SameLine();


		if (ImGui::InputText("DevConsoleView/Cmd", mCmdStr, mCmdStr) == true)
		{
			if (ImGui::IsKeyPressed((i32)EKeyCode::Enter) == true)
			{
			
				CommandExecution(mCmdStr);
				mCmdStr.clear();
				
				ImGui::SetKeyboardFocusHere(1);
					
			}

		}
		ImGui::EndChild();
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
				ReadLog_Async();
			});
		}
	}

	void DevConsoleView::ReadLog_Async()
	{
		u32 readLineCount = 0;
		Queue<String>& messageQueue = mSink->GetLogMessageQueue();
		std::lock_guard<std::mutex> lock(mSink->GetMutex());
		while (messageQueue.empty() == false)
		{
			String log = messageQueue.front(); messageQueue.pop();
			PushLogInfo_Async(log);
			++readLineCount;
			if (readLineCount > mReadMaxLogLine)
			{
				break;
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
			info.LogLevel = ELogLevel::Trace;
			info.Message = StringHelper::ReplaceAll(info.Message, "[trace]", "");
		}
		else if (msg.find("[info]") != String::npos)
		{
			info.LogLevel = ELogLevel::Info;
			info.Message = StringHelper::ReplaceAll(info.Message, "[info]", "");
		}
		else if (msg.find("[warning]") != String::npos)
		{
			info.LogLevel = ELogLevel::Warn;
			info.Message = StringHelper::ReplaceAll(info.Message, "[warning]", "");
		}
		else if (msg.find("[error]") != String::npos)
		{
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
	void DevConsoleView::CommandExecution(const String& command)
	{

		mCommandPrompt->Execute(command);
	}
}