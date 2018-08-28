#include"Object.h"
#include"../Components/Component.h"
#include"../Components/MotivatedComponent.h"

Object::Object()
{
	RegisterObjectID(typeid(this));
	std::unique_ptr<Component> RootComponent;
}

Object::~Object()
{

}

void Object::BeginObject()
{

}

void Object::Send(JGComponentMessage & Message)
{

}

void Object::Tick(const float DeltaTime)
{

}

void Object::Render(JGDevice* Device, JGHLSLShaderDevice* HlslDevice)
{

}

void Object::DestroyObject()
{

}

template<typename ComponentType>
typename ComponentType* Object::RegisterComponentInObject(std::wstring& ComponentName)
{
	// 만약 처음 등록하는 컴포넌트라면..
	if (m_bIsFirst)
	{
		m_bIsFirst = false;
		// 루트 컴포넌트를 만든다.
		std::unique_ptr<Component> RootComponent = make_unique<Component>();
		m_RootComponent = RootComponent.get();
		m_vComponents.push_back(move(RootComponent));
	}
	// 컴포넌트 생성
	std::unique_ptr<Component> component = make_unique<ComponentType>();
	component->RegisterName(ComponentName);

	// 계층 구조 컴포넌트인지 체크한다.
	MotivatedComponent* Check = dynamic_cast<MotivatedComponent*>(component.get());
	// 계층 구조 컴포넌트라면.
	if (Check)
	{
		// 루트 컴포넌트 자식으로 컴포넌트를 추가한다.
		ComponentType* result = Check.get();
		m_RootComponent->AddChild(component.get());
		m_vComponents.push_back(move(component));
		return result;
	}
	else
	{
		// 아니라면 그냥 추가한다.
		ComponentType* result = Check.get();
		m_vComponents.push_back(move(component));
		return result;
	}
}
