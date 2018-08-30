#include"Object.h"
#include"../Components/MotivatedComponent.h"

Object::Object()
{
	RegisterObjectID(typeid(this));
}

Object::~Object()
{

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

void Object::DestroyObject()
{
	if (m_RootComponent)
	{
		m_RootComponent->DestoryComponent();
	}
}


