#include<PCH.h>
#include"Gui_Log.h"
#include<Util/StringUtil.h>


using namespace std;
using namespace JgGui;
using namespace Common;

Gui_Log::Gui_Log(const std::string& name) : GuiComponent(name)
{
	m_FilteringStr.resize(m_FilteringStrBufferSize);
	m_EmptyStr.resize(m_FilteringStrBufferSize);

	m_ColorByLogLevel[Log::Info] = { 0.0f,1.0f,0.0f,1.0f };
	m_ColorByLogLevel[Log::Default] = { 1.0f,1.0f,1.0f,1.0f };
	m_ColorByLogLevel[Log::Warning] = { 1.0f, 1.0f,0.0f,1.0f };
	m_ColorByLogLevel[Log::Error] = { 1.0f, 0.5f,0.0f,1.0f };
	m_ColorByLogLevel[Log::Critical] = { 1.0f,0.0f,0.0f,1.0f };



	m_LogLevelTable.push_back(m_AllLogLevelKey);
	for (int i = 0; i < Log::LogLevel_Num; ++i)
	{
		m_LogLevelTable.push_back(Log::LogLevelToString((Log::ELogLevel)i));
	}

	m_ThreadGroupTable[0] = m_AllThreadKey;
	m_LogGroupNameTable.insert(m_AllGroupKey);
}

Gui_Log::~Gui_Log() {}


void Gui_Log::NewFrame()
{
	
	Log::EngineLog* log = Log::GetLogAddress();

	while (!log->Empty())
	{
		Log::LogMessage msg;
		log->TryPop(msg);

		auto txt = make_unique<Text>();
		txt->Color = m_ColorByLogLevel[msg.level];
		txt->Str   = msg.str;
		txt->line  = m_LineOffset++;
		txt->Level = msg.level;
		txt->GroupName = msg.groupName;
		txt->ThreadID = msg.threadID;

		Text* ptxt = txt.get();

		m_AllTextBuffer.push_back(move(txt));
		FilterLog(ptxt, msg);

		if (m_ThreadGroupTable.find(msg.threadID) == m_ThreadGroupTable.end())
		{
			m_ThreadGroupTable[msg.threadID] = to_string(msg.threadID);
		}
		if (m_LogGroupNameTable.find(msg.groupName) == m_LogGroupNameTable.end())
		{
			m_LogGroupNameTable.insert(msg.groupName);
		}
		m_ScrollToBottom = true;
	}
	// GUI
	if (m_IsVisible)
	{
		GuiComponent::NewFrame();
		ImGui::Begin(m_GuiName.c_str(), &m_IsVisible);

		// LogGroup 필터링
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.3f);
		if (ImGui::BeginCombo("##LogGroup", m_Selected_LogGroupName.c_str()))
		{

			for (auto& GroupName : m_LogGroupNameTable)
			{
				bool is_select_item = (m_Selected_LogGroupName == GroupName);
				if (ImGui::Selectable(GroupName.c_str(), &is_select_item))
				{
					m_Selected_LogGroupName = GroupName;
				}
				if (is_select_item)
					ImGui::SetItemDefaultFocus();

			}

			ImGui::EndCombo();
		}
		// LogThread 필터링
		ImGui::SameLine();
		if (ImGui::BeginCombo("##ThreadGroup", m_Selected_LogThread.c_str()))
		{

			for (auto& thread : m_ThreadGroupTable)
			{
				bool is_select_item = (m_Selected_LogThread == thread.second);

				if (ImGui::Selectable(thread.second.c_str(), &is_select_item))
				{
					m_Selected_LogThread = thread.second;
				}
				if (is_select_item)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		// 로그 레벨 필터링
		ImGui::SameLine();
		if (ImGui::BeginCombo("##LogLevelGroup", m_Selected_LogLevel.c_str()))
		{
			for (size_t i = 0; i < m_LogLevelTable.size(); ++i)
			{
				bool is_select_item = (m_Selected_LogLevel == m_LogLevelTable[i]);
				if (ImGui::Selectable(m_LogLevelTable[i].c_str(), &is_select_item))
				{
					m_Selected_LogLevel = m_LogLevelTable[i];
				}
				if(is_select_item)
					ImGui::SetItemDefaultFocus();

			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
		// 텍스트 필터링
		ImGui::PushItemWidth(ImGui::GetWindowWidth());
		ImGui::InputText("##LogFilter", &m_FilteringStr[0], m_FilteringStrBufferSize);
		ImGui::PopItemWidth();
		ImGui::Separator();

		//
		ImGui::BeginChild("scrolling");
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));


		// 로그 출력
		for (auto& log : m_FilteredTextBuffer[m_Selected_LogGroupName][m_Selected_LogThread][m_Selected_LogLevel])
		{
			PrintLog(log);
		}

		if (m_ScrollToBottom)
		{
			ImGui::SetScrollHere(1.0f);
			m_ScrollToBottom = false;
		}
		ImGui::PopStyleVar();
		ImGui::EndChild();
		ImGui::End();
	}
}
void Gui_Log::FilterLog(Text* log, const Log::LogMessage& msg)
{
	string threadStr  = to_string(msg.threadID);
	string loglevelStr = Log::LogLevelToString(msg.level);
	m_FilteredTextBuffer[msg.groupName][threadStr][loglevelStr].push_back(log);
	m_FilteredTextBuffer[msg.groupName][threadStr][m_AllLogLevelKey].push_back(log);
	m_FilteredTextBuffer[msg.groupName][m_AllThreadKey][loglevelStr].push_back(log);
	m_FilteredTextBuffer[m_AllGroupKey][threadStr][loglevelStr].push_back(log);
	m_FilteredTextBuffer[msg.groupName][m_AllThreadKey][m_AllLogLevelKey].push_back(log);
	m_FilteredTextBuffer[m_AllGroupKey][threadStr][m_AllLogLevelKey].push_back(log);
	m_FilteredTextBuffer[m_AllGroupKey][m_AllThreadKey][loglevelStr].push_back(log);
	m_FilteredTextBuffer[m_AllGroupKey][m_AllThreadKey][m_AllLogLevelKey].push_back(log);
}
void Gui_Log::PrintLog(Text* log)
{
	if (m_FilteringStr != m_EmptyStr)
	{
		// 모두 소문자로 변환후 비교
		size_t pos = m_FilteringStr.find('\0');
		string noneSpaceStr = m_FilteringStr.substr(0, pos);
		string originStr;
		originStr.resize(log->Str.size());
		std::transform(noneSpaceStr.begin(), noneSpaceStr.end(), noneSpaceStr.begin(),
			::tolower);
		std::transform(log->Str.begin(), log->Str.end(), originStr.begin(),
			::tolower);
		if (Util::IsExsitString(originStr, noneSpaceStr))
		{
			ImGui::TextColored(log->Color, log->Str.c_str());
		}
	}
	else
	{
		ImGui::TextColored(log->Color, log->Str.c_str());
	}
}