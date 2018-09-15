#pragma once
#include"../../EngineStatics/Engine.h"

class Object;
class RenderSystem;
/*
Class : World 
@m m_WorldName : 월드 이름 
@m vector<shared_ptr<Object>> m_vObjects : 오브젝트 배열 */
class ENGINE_EXPORT World
{
private:
	std::wstring m_WorldName = TT("None");
	std::list<std::shared_ptr<Object>> m_sObjects;
public:
	World() = delete;
	World(const std::wstring& Name);
	~World();
	/*
	Exp : 프레임 1당 호출되는 함수 */
	void Tick(const float DeltaTime);
	/*
	Exp : 현제 월드 상에 존재하는 오브젝의 배열을 가져온다. */
	std::list<std::shared_ptr<Object>>* GetObjectArray();
};
