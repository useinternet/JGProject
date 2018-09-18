#pragma once
#include"../../EngineStatics/Engine.h"

class World;
class JGPhysicsSystem;
/*
Class : WorldManager 
@m map<const wstring, shared_ptr<World>> m_mWorlds : 존재하는 월드 배열
@m wstring m_CurrentWorld : 현제 월드 이름
@m wstring m_PrevWorld : 이전 현제 월드 이름
@m bool m_bWorldChange : 현제 월드가 바뀌었는지 여부 */
class ENGINE_EXPORT WorldManager
{
private:
	std::map<const std::wstring, std::shared_ptr<World>> m_mWorlds;
	std::wstring m_CurrentWorld = TT("None");
	std::wstring m_PrevWorld    = TT("None");
	JGPhysicsSystem* m_pPhysicsSystem;
	bool m_bWorldChange = true;
public:
	WorldManager();
	~WorldManager();
	void InitWorldManager(JGPhysicsSystem* pySystem);
	/*
    Exp : 1 프레임당 한번 호출되는 함수 */
	void Tick(const float DeltaTime);
	/*
	Exp : 현제 월드를 정한다. */
	void SelectWorld(const std::wstring& worldName);
	/*
	Exp : 월드를 추가한다. */
	void AddWorld(const std::wstring& worldName);
	/*
	Exp : 해당이름을 가진 월드를 가져온다.. */
	World* GetWorld(const std::wstring& WorldName);
	/*
	Exp : 현재 월드를 가져온다. */
	World* GetCurrentWorld();
	/*
	Exp : 현재 월드 이름을 가져온다.*/
	const std::wstring& GetCurrentWorldName();
	/*
	Exp : 현재 월드가 바뀌었는지 여부 */
	bool GetIsChangeWorld();
};