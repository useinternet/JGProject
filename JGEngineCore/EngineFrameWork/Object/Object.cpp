#include"Object.h"
#include"../Components/MotivatedComponent.h"
#include"../World/World.h"
float Object::m_DeltaTime = 0.0f;
Object::Object()
{
	RegisterObjectID(typeid(this));
	m_ObjectState = EObjectState::Active;
}

Object::~Object()
{
	int num = 0;
}

void Object::BeginObject(World* world)
{
	m_pWorld = world;
	for (auto& iter : m_vSystemComponents)
	{
		iter->BeginComponent(world);
	}
	if (m_RootComponent)
	{
		m_RootComponent->BeginComponent(world);
	}
}

void Object::Send(JGComponentMessage & Message)
{

}

void Object::Tick(const float DeltaTime)
{
	m_DeltaTime = DeltaTime;
	for (auto& iter : m_vSystemComponents)
	{
		iter->Tick(DeltaTime);
	}
	if (m_RootComponent)
	{
		m_RootComponent->Tick(DeltaTime);
	}
}

void Object::Render()
{
	for (auto& iter : m_vSystemComponents)
	{
		iter->Render();
	}
	if (m_RootComponent)
	{
		m_RootComponent->Render();
	}
}

void Object::EndObject()
{
	for (auto& iter : m_vSystemComponents)
	{
		iter->DestoryComponent();
	}
	if (m_RootComponent)
	{
		m_RootComponent->DestoryComponent();
	}
}
World* Object::GetWorld()
{
	return m_pWorld;
}
EObjectState Object::GetObjectState()
{
	return m_ObjectState;
}
void Object::DestoryObject()
{
	m_ObjectState = EObjectState::Destory;
}
void Object::BehindObject()
{
	m_ObjectState = EObjectState::Behind;
}

void Object::ActiveObject()
{
	m_ObjectState = EObjectState::Active;
}

float Object::GetDeltaTime()
{
	return m_DeltaTime;
}
