#include"World.h"
#include"../Object/Object.h"
#include"../Object/ExistObject.h"
#include"../../Test/TestWidget.h"
using namespace std;
World::World(const std::wstring& Name)
{
	// 임시로 그냥 생성한다.
	shared_ptr<Object> object = make_unique<ExistObject>();
	object->BeginObject();
	m_sObjects.push_back(move(object));
	shared_ptr<Object> Widget = make_unique<TestWidget>();
	Widget->BeginObject();
	Widget->ActiveObject();
	m_sObjects.push_back(move(Widget));

}
World::~World()
{

}
void World::Tick(const float DeltaTime)
{
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