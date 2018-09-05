#pragma once
#include"../EngineStatics/Engine.h"


// 할려는 거 정리
// JGPressManager : 그냥 키 받아서 눌렷는지만 판단..
// JGCommand : 커맨드 이름하구 키 저장해서 이게 눌렀는지( 유지보수용 클래스)
// JGKeyCommand : 키가 눌렷는지 똇는지를 구별해서 그에맞게 이벤트 
// JGAxisCommand : 해당 키에 지속적으로값을 주입하는거..


class JGPressManager;
enum class EKey;
/*
Class : JGCommand
@m unique_ptr<wstring> m_CommandName : 바인딩할 커맨트 이름.
@m EKey m_Key : 바인딩 할 키..
@m bool m_bIsPressed : 키가 눌렸는지.. */
class ENGINE_EXPORT JGCommand
{
private:
	std::unique_ptr<std::wstring> m_CommandName;
	EKey  m_Key;
	bool m_bIsPressed = false;


	JGPressManager* m_pPressManager = nullptr;
public:
	JGCommand(const JGCommand& copy) = delete;
	JGCommand();
	virtual ~JGCommand();
public:
	/*
	Exp : 커맨드 등록 
	@param JGPressManager* PressManager : JGPressManager 클래스 포인터
	@param const EKey key : 바인딩 할 키
	@param const wstring CommandName : 바인딩할 커맨드 이름 */
	void RegisterCommand(JGPressManager* PressManager, const EKey key, const std::wstring& CommandName);
	/*
	Exp : 1프레임당 한번 호출하는 함수. */
	virtual void Tick();
	/*
	Exp : 바인딩 된 Key를 가져온다. */
	EKey GetKey();
	/*
	Exp : 바인딩할 Key설정한다. */
	void SetKey(EKey key);
	/*
	Exp : Key가 눌렸는지 여부 */
	bool GetIsPressed();
	/*
	Exp : 바인딩 된 커맨드 이름을 가져온다. */
	const std::wstring& GetCommandName();
	/*
	Exp : 바인딩 할 커맨드 이름 셋팅한다.*/
	void SetCommandName(const std::wstring& CommandName);
	/*
	Exp : PressManager를 가져온다. */
	JGPressManager* GetPressManager();
};