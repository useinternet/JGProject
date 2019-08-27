#include "pch.h"
#include "ConsoleLog.h"

namespace JE
{
	void ConsoleLog::OnLoad()
	{
		SetWindowFlags(ImGuiWindowFlags_NoScrollbar);
		

		m_LogfileCurrPos = 0;
		AsyncEvent e;
		e.Function = [&]()
		{
			m_Fin.open(Log::GetFileName());
			if (!m_Fin.is_open())
			{
				ENGINE_LOG_ERROR("{0} can't find logfile", GetName());
				return;
			}

			m_Fin.seekg(0, std::ios::end);
			if (m_LogfileCurrPos == m_Fin.tellg())
			{
				m_Fin.close();
				return;
			}

			m_Fin.seekg(0, std::ios::beg);
			m_Fin.seekg(m_LogfileCurrPos, std::ios::beg);

			while (!m_Fin.eof())
			{
				m_LogfileCurrPos = m_Fin.tellg();
				std::string str;
				std::getline(m_Fin, str);

				size_t start_offset = str.find('[') + 1;
				size_t end_offset = str.find(']');
				std::string log_level = str.substr(start_offset, end_offset - start_offset);
				if (log_level == "info") m_DumpLogLevel.push_back(LOGLEVEL_INFO);
				else if (log_level == "warning") m_DumpLogLevel.push_back(LOGLEVEL_WARNING);
				else if (log_level == "error") m_DumpLogLevel.push_back(LOGLEVEL_ERROR);
				else if (log_level == "trace") m_DumpLogLevel.push_back(LOGLEVEL_TRACE);
				else if (log_level == "critical") m_DumpLogLevel.push_back(LOGLEVEL_FATAL);
				m_DumpStringArrays.push_back(str);
			}
			m_Fin.close();
		};
		m_LoadLogfileEventHandler = GlobalLinkData::EngineEventManager->GetAsyncEventHandler(e);
		m_LoadLogfileEventHandler.Excute();




		m_TextColor[LOGLEVEL_TRACE] = { 1.0f,1.0f,1.0f,1.0f };
		m_TextColor[LOGLEVEL_INFO] = { 0.0f,1.0f,0.0f,1.0f };
		m_TextColor[LOGLEVEL_WARNING] = { 1.0f,1.0f,0.0f,1.0f };
		m_TextColor[LOGLEVEL_ERROR] = { 1.0f,0.5f,0.0f,1.0f };
		m_TextColor[LOGLEVEL_FATAL] = { 1.0f,0.0f,0.0f,1.0f };
		//
		m_AutoScroll = true;
		Clear();
	}
	void ConsoleLog::OnGUI()
	{
		if (m_LoadLogfileEventHandler.IsComplete())
		{
			if (!m_DumpStringArrays.empty())
			{
				uint32_t count = 0;
				for (auto& str : m_DumpStringArrays)
				{
					if (str.empty()) continue;
					m_StringArray.push_back(str);
					m_LogLevel.push_back(m_DumpLogLevel[count]);
					++count;
				}
				m_DumpStringArrays.clear();
				m_DumpLogLevel.clear();
			}
			m_LoadLogfileEventHandler.Reset();
			m_LoadLogfileEventHandler.Excute();
		}
		Draw();
	}
	void ConsoleLog::OnDestroy()
	{


	}

	void ConsoleLog::Clear()
	{
		m_StringArray.clear();
		m_LogLevel.clear();
	}
	void ConsoleLog::Draw()
	{
		ImVec2 windowSize = ImGui::GetWindowSize();
		ImGui::BeginChild("scrolling", ImVec2(windowSize.x, windowSize.y - 60.0f), false, ImGuiWindowFlags_HorizontalScrollbar);
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			if (m_TextFilter.IsActive())
			{
				uint32_t count = 0;
				for (auto& str : m_StringArray)
				{
					const char* line_start = str.c_str();
					const char* line_end =  (&str[0]) + str.length();


					if (m_TextFilter.PassFilter(line_start, line_end))
					{
						ImGui::TextColored(m_TextColor[m_LogLevel[count++]], line_start, line_end);
					}
				}
			}
			else
			{
				uint32_t count = 0;
				for (auto& str : m_StringArray)
				{
					ImGui::TextColored(m_TextColor[m_LogLevel[count++]], str.c_str());
				}
			}
			ImGui::PopStyleVar();
			if (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);
		}

		ImGui::EndChild();
		ImGui::Separator();


		// filter
		{
			m_TextFilter.Draw("##Filter", -200.0f);
			ImGui::SameLine();
			bool clear = ImGui::Button("Clear");
			ImGui::SameLine();
			ImGui::Checkbox("Auto-scroll", &m_AutoScroll);
			if (clear)
				Clear();
		}
	}
}