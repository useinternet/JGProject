#include"Object.h"
#include"../Components/MotivatedComponent.h"

Object::Object()
{
	RegisterObjectID(typeid(this));
	m_ObjectState = EObjectState::Active;
}

Object::~Object()
{
	int num = 0;
}

void Object::BeginObject()
{
	if (m_RootComponent)
	{
		m_RootComponent->BeginComponent();
	}
}

void Object::Send(JGComponentMessage & Message)
{

}

void Object::Tick(const float DeltaTime)
{
	if (m_RootComponent)
	{
		m_RootComponent->Tick(DeltaTime);
	}
}

void Object::Render()
{
	if (m_RootComponent)
	{
		m_RootComponent->Render();
	}
}

void Object::EndObject()
{
	if (m_RootComponent)
	{
		m_RootComponent->DestoryComponent();
	}
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
