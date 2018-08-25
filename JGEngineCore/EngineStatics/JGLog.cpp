#include"JGLog.h"
#include"JThreadManager.h"

using namespace std;
#ifdef _DEBUG
std::queue<JGLog::SLogMessage> JGLog::m_MessageQue_1;
std::queue<JGLog::SLogMessage> JGLog::m_MessageQue_2;
bool JGLog::m_bWorking_1 = false;
bool JGLog::m_bWorking_2 = true;
JGLog::CurrentWorkingQue JGLog::m_CurrentWorkingQue = JGLog::CurrentWorkingQue::MessageQue_2;
//                                   R     G     B     A
D3DXCOLOR JGLog::Default_Color  = { 0.0f ,0.0f ,0.0f ,1.0f };
D3DXCOLOR JGLog::Warning_Color  = { 1.0f ,1.0f ,0.0f ,1.0f };
D3DXCOLOR JGLog::Error_Color    = { 1.0f ,0.0f ,0.0f ,1.0f };
D3DXCOLOR JGLog::Progress_Color = { 0.0f ,1.0f ,0.0f ,1.0f };
std::wstring JGLog::Default_str = TT("Log : ");
std::wstring JGLog::Warning_str = TT("Warning : ");
std::wstring JGLog::Error_str   = TT("Error : ");
std::wstring JGLog::Progress_str= TT("Progress : ");
#endif

JGLog::JGLog()
{
}
JGLog::~JGLog()
{

}
void JGLog::InitLog()
{
#ifdef _DEBUG
	JThreadManager::AddTask([this]
	{
		while (m_IsRunning)
		{
			Tick();
		}
	});
#endif
}
void JGLog::Write(ELogLevel level, const wchar_t* LogMessage, ...)
{
#ifdef _DEBUG
	// 가변인자 받아서 처리
	WCHAR str[256];
	va_list ArgList;
	va_start(ArgList, LogMessage);
	wvsprintf(str,LogMessage, ArgList);
	va_end(ArgList);
	wstring result = str;


	// 레벨에 따른 색 및 메세지 창 수정
	SLogMessage log;
	switch (level)
	{
	case ELogLevel::Default:
		log.Color   = Default_Color;
		log.Message = Default_str + result + TT("\n");
		break;
	case ELogLevel::Warning:
		log.Color = Warning_Color;
		log.Message = Warning_str + result + TT("\n");
		break;
	case ELogLevel::Error:
		log.Color = Error_Color;
		log.Message = Error_str + result + TT("\n");
		break;
	case ELogLevel::Progress:
		log.Color = Progress_Color;
		log.Message = Progress_str + result + TT("\n");
		break;
	}
	// 로그 큐에 저장
	if (m_CurrentWorkingQue == MessageQue_2)
	{
		m_MessageQue_1.push(log);
	}
	else if (m_CurrentWorkingQue == MessageQue_1)
	{
		m_MessageQue_2.push(log);
	}
#endif
}
void JGLog::Destory()
{
#ifdef _DEBUG
	m_IsRunning = false;
#endif
}
void JGLog::Tick()
{
#ifdef _DEBUG
	if (!m_MessageQue_1.empty() && !m_bWorking_1)
	{
		m_bWorking_1 = true;
		m_CurrentWorkingQue = MessageQue_1;
		JThreadManager::AddTask([this]()
		{
			while (!m_MessageQue_1.empty())
			{
				// 로그 출력(일단 밑 출력창으로 출력
				OutputDebugString(m_MessageQue_1.front().Message.c_str());
				m_MessageQue_1.pop();
			}
			m_bWorking_2 = false;
		});
	}
	if (!m_MessageQue_2.empty() && !m_bWorking_2)
	{
		m_bWorking_2 = true;
		m_CurrentWorkingQue = MessageQue_2;

		JThreadManager::AddTask([this]()
		{
			while (!m_MessageQue_2.empty())
			{
				// 로그 출력(일단 밑 출력창으로 출력
				OutputDebugString(m_MessageQue_2.front().Message.c_str());
				m_MessageQue_2.pop();
			}
			m_bWorking_1 = false;
		});
	}
#endif
}

