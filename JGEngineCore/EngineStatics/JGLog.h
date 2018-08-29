#pragma once
#include"Engine.h"


/*
EnumClass : ELogLevel
@e Default : 기본 로그( 검은색 )
@e Warning : 경고 로그( 노란색 )
@e Error   : 에러 로그( 빨간색 )
@e Progerss: 진행 로그( 초록색 )
*/
enum class ENGINE_EXPORT ELogLevel
{
	Default,    // 검은색
	Warning,    // 노란색
	Error,      // 빨간색
	Progress,   // 초록색
};
/*
Class : JGLog
@m	static std::queue<SLogMessage> m_MessageQue_1 : 메세지로그 큐 1
@m	static std::queue<SLogMessage> m_MessageQue_2 : 메세지로그 큐 2
@m  bool m_IsRunning : 활성화 여부
*/
class ENGINE_EXPORT JGLog
{
private:
	enum CurrentWorkingQue
	{
		MessageQue_1,
		MessageQue_2
	};
	struct SLogMessage
	{
		D3DXCOLOR Color   = { 0.0f,0.0f,0.0f,0.0f };
		std::wstring Message = TT("None");
	};
private: 
#ifdef _DEBUG
	static std::queue<SLogMessage> m_MessageQue_1;
	static std::queue<SLogMessage> m_MessageQue_2;
	bool m_IsRunning = true;

#endif
public:
	JGLog();
	~JGLog();
	/* 
	Exp : 로그를 활성화한다.*/
	void InitLog();
	/*
	Exp : 로그 작성
	@param ELogLevel level : 로그 레벨
	@param const wchar_t* LogMessage : 로그 메세지
	@param ... : 가변인자    */
	static void Write(ELogLevel level,const wchar_t* LogMessage,...);
	/*
	Exp : 로그를 파괴한다..*/
	void Destory();
private:
	void Tick();
	void OutputMessage();
private: // 로그 설정 전역 변수들..
	static bool m_bWorking_1;
	static bool m_bWorking_2;
	static CurrentWorkingQue m_CurrentWorkingQue;
	static D3DXCOLOR Default_Color;
	static D3DXCOLOR Warning_Color;
	static D3DXCOLOR Error_Color;
	static D3DXCOLOR Progress_Color;
	static std::wstring Default_str;
	static std::wstring Warning_str;
	static std::wstring Error_str;
	static std::wstring Progress_str;
};