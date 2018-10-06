#pragma once
#include"../../EngineStatics/Engine.h"

class Object;
class RenderSystem;
class JGMatrix;
class JGPhysicsWorld;
class WorldManager;
/*
Class : World 
@m m_WorldName : 월드 이름 
@m vector<shared_ptr<Object>> m_vObjects : 오브젝트 배열 */
class ENGINE_EXPORT World
{
private:
	WorldManager* m_WorldManager;
private:
	std::wstring m_WorldName = TT("None");
	std::list<std::shared_ptr<Object>> m_sObjects;
	JGPhysicsWorld* m_pyWorld = nullptr;
	std::unique_ptr<JGMatrix> m_ViewMatrix;
public:
	World() = delete;
	World(const std::wstring& Name, JGPhysicsWorld* pyWorld,WorldManager* manager);
	~World();

	/*
	Exp : 프레임 1당 호출되는 함수 */
	void Tick(const float DeltaTime);
	/*
	Exp : 현제 월드 상에 존재하는 오브젝의 배열을 가져온다. */
	std::list<std::shared_ptr<Object>>* GetObjectArray();

	template<typename ObjectType>
	Object* SpawnObject();

	void TempViewMatrixInit();
	JGMatrix& GetViewMatrix();
	JGPhysicsWorld* GetPyWorld();
	void Clear();
	WorldManager* GetManager();
};

template<typename ObjectType>
inline Object* World::SpawnObject()
{
	std::shared_ptr<Object> object = std::make_unique<ObjectType>();
	object->BeginObject(this);
	Object* result = object.get();
	m_sObjects.push_back(move(object));
	return result;
}
