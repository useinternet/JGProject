#pragma once
#include"../EngineStatics/Engine.h"

class JGPressManager;
class JGCommand;
class JGVector2D;
enum class EKey;
enum class EKeyState;

/*
Class : JGCommandManager 
@m map<wstring, shared_ptr<JGCommand>> m_mCommands : 관리하는 커맨드 배열 */
class ENGINE_EXPORT JGCommandManager
{
private:
	JGPressManager* m_pPressManager = nullptr;
	std::map<std::wstring, std::shared_ptr<JGCommand>> m_mCommands;
public:
	JGCommandManager();
	virtual ~JGCommandManager();
	/*
	Exp : 커맨드 매니저 초기화 */
	void InitCommandManager(JGPressManager* Manager);
	/*
	Exp : 1프레임당 한번 호출하는 함수 */
	void Tick();
	/*
	Exp : 키 등록 
	@param const EKey key : 바인딩할 키 
	@param const wstring& CommandName : 바인딩할 커맨드 이름 */
	void RegisterKeyCommand(const EKey key, const std::wstring& CommandName);
	/*
	Exp : 키 커맨드 이벤트 바인딩 
	@param const wstring& CommandName : 바인딩한 커맨드 이름 
	@param const EKeyState KeyState : 키 상태(누름 , 똄) 
	@param const function<void()>& Event : void() 형 바인딩할 이벤트 */
	void BindKeyCommand(const std::wstring& CommandName, const EKeyState KeyState, const std::function<void()>& Event);
	/*
	Exp : 엑시스 커맨드 이벤트 바인딩
	@param const wstring& CommandName : 바인딩한 커맨드 이름
	@param const function<void(const float)>& Event : void(const float) 형 바인딩할 이벤트 */
	void BindAxisCommand(const std::wstring& CommandName, const std::function<void(const float)>& Event);


	void KillKeyCommand(const std::wstring& CommandName);
	void KillAxisCommand(const std::wstring& CommandName);
	/*
	Exp : 마우스 좌표를 가져온다. */
	JGVector2D& GetMouseLocation();
};