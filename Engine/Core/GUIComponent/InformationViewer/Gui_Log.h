#pragma once
#include"GuiComponent.h"
#include<Data/Log.h>

namespace JgGui
{
	class Gui_Log : public GuiComponent
	{
		struct Text
		{
			int line = 0;
			std::string GroupName;
			uint32_t ThreadID;
			Common::Log::ELogLevel Level;
			ImVec4 Color = { 0.0f,0.0f,0.0f,1.0f };
			std::string Str = "";
		};
		typedef std::vector<std::unique_ptr<Text>>       TextBuffer;
		typedef std::vector<Text*>                       TextPointerBuffer;
		typedef std::unordered_map<std::string,
			    std::unordered_map<std::string,
			    std::unordered_map<std::string, TextPointerBuffer>>> FilteredTextBuffer;
	private:
		// 로그 텍스트 퍼버
		uint32_t           m_LineOffset = 0;
		TextBuffer         m_AllTextBuffer;
		FilteredTextBuffer m_FilteredTextBuffer;
		// 로그 그룹 / 쓰레드 / 로그 레벨 필터링 변수
		std::string m_AllGroupKey    = "All Group";
		std::string m_AllThreadKey   = "All Thread";
		std::string m_AllLogLevelKey = "All LogLevel";
		//
		std::string m_Selected_LogGroupName = m_AllGroupKey;
		std::set<std::string> m_LogGroupNameTable;
		//
		std::string m_Selected_LogThread = m_AllThreadKey;
		std::map<uint32_t, std::string> m_ThreadGroupTable;
		// 
		std::string m_Selected_LogLevel = m_AllLogLevelKey;
		std::vector<std::string> m_LogLevelTable;
		// 로그 텍스트 필터링 변수
		std::string m_FilteringStr;
		std::string m_EmptyStr;
		static const uint32_t m_FilteringStrBufferSize = 256;
		// 기타 설정 변수
		ImVec4 m_ColorByLogLevel[Common::Log::LogLevel_Num];
		bool   m_ScrollToBottom = false;
	public:
		Gui_Log(const std::string& name = "Log");
		virtual ~Gui_Log();
	public:
		virtual void NewFrame() override;
	private:
		void FilterLog(Text* log, const Common::Log::LogMessage& msg);
		void PrintLog(Text* log);
	};
}