#include"World.h"
#include"../../PhysicsSystem/PhysicsWorld.h"
#include"../Object/Object.h"
#include"../Object/ExistObject.h"
#include"../../Test/TestWidget.h"

using namespace std;
World::World(const std::wstring& Name)
{
	m_PhysicsWorld = make_unique<PhysicsWorld>();
	// 임시로 그냥 생성한다.
	shared_ptr<Object> object = make_unique<ExistObject>();
	object->BeginObject(this);
	m_sObjects.push_back(move(object));
	shared_ptr<Object> Widget = make_unique<TestWidget>();
	Widget->BeginObject(this);
	Widget->ActiveObject();
	m_sObjects.push_back(move(Widget));
}
World::~World()
{

}
void World::Tick(const float DeltaTime)
{
	m_PhysicsWorld->Tick(DeltaTime, DeltaTime);
	for (auto object = m_sObjects.begin(); object != m_sObjects.end();)
	{
		if ((*object)->GetObjectState() != EObjectState::Destory)
		{
			(*object)->Tick(DeltaTime);
			object++;
		}
		else
		{
			object = m_sObjects.erase(object);
		}
	}
	
}
list<shared_ptr<Object>>* World::GetObjectArray()
{
	return &m_sObjects;
}

PhysicsWorld * World::GetPhysicsWorld()
{
	return m_PhysicsWorld.get();
}
