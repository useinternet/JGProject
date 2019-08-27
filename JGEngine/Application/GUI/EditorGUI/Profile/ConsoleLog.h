#pragma once
#include "GUI/EditorGUI/IGWindow.h"
namespace JE
{
	class ConsoleLog : public IGWindow
	{
		enum
		{
			LOGLEVEL_TRACE,
			LOGLEVEL_INFO,
			LOGLEVEL_WARNING,
			LOGLEVEL_ERROR,
			LOGLEVEL_FATAL,
			LOGLEVEL_NUM
		};
		std::ifstream   m_Fin;
		std::streampos  m_LogfileCurrPos;
		std::vector<std::string> m_StringArray;
		std::vector<std::string> m_DumpStringArrays;
		ImVector<int>   m_DumpLogLevel;
		ImVector<int>   m_LogLevel;
		AsyncEventHandler m_LoadLogfileEventHandler;

		// Log에 필요한 변수

		ImGuiTextFilter m_TextFilter;

		ImColor m_TextColor[LOGLEVEL_NUM];
		bool            m_AutoScroll;
	protected:
		virtual void OnLoad() override;
		virtual void OnGUI() override;
		virtual void OnDestroy() override;
	private:
		void Clear();
		void Draw();

	};
}