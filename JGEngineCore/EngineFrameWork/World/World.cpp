#include"World.h"
#include"../Object/Object.h"
#include"../Object/ExistObject.h"
using namespace std;
World::World(const std::wstring& Name)
{
	// 임시로 그냥 생성한다.
	shared_ptr<Object> object = make_unique<ExistObject>();
	object->BeginObject();
	m_sObjects.push_back(move(object));
	// 임시로 그냥 생성한다.
	shared_ptr<Object> object2 = make_unique<ExistObject>();
	object2->BeginObject();
	m_sObjects.push_back(move(object2));
	// 임시로 그냥 생성한다.
	shared_ptr<Object> object3 = make_unique<ExistObject>();
	object3->BeginObject();
	m_sObjects.push_back(move(object3));
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

/*
for (it = numbers.begin(); it != numbers.end(); ) 
{ 
    if (*it % 2 == 0)
	{ 
	   it = numbers.erase(it);
	} 
	else { ++it; } 
}
*/