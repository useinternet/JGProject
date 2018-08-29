#include "Component.h"
#include"../../RenderSystem/JGDeviceD.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGHLSLShaderDevice.h"
using namespace std;
Component::Component()
{
	RegisterComponentID(typeid(this));

}

Component::~Component()
{
}

void Component::Receive(const JGComponentMessage& Message)
{
	for (auto& iter : m_vChildComponent)
	{
		iter->Receive(Message);
	}
}

void Component::BeginComponent()
{
	for (auto& iter : m_vChildComponent)
	{
		iter->BeginComponent();
	}
}

void Component::Tick(const float DeltaTime)
{
	for (auto& iter : m_vChildComponent)
	{
		iter->Tick(DeltaTime);
	}
}

void Component::Render()
{
	for (auto& iter : m_vChildComponent)
	{
		iter->Render();
	}
}

void Component::DestoryComponent()
{

}


void Component::RegisterName(const wstring& Name)
{
	m_ComponentName = Name;
}
const wstring& Component::GetComponentName()
{
	return m_ComponentName;
}

void Component::AddChild(Component* Child)
{
	Component* ParentOfChild = Child->GetParent();
	// 만약 추가해야할 자식 컴포넌트에 부모 컴포넌트가 있다면.. 
	if (ParentOfChild)
	{
		Component* TempChild = Child;
		// 부모 컴포넌트에있는 자식배열에서 삭제한다. 그후..
		for (auto iter = ParentOfChild->GetChildArray().begin();
			      iter < ParentOfChild->GetChildArray().end(); )
		{
			if (Child == *iter)
			{
				iter = ParentOfChild->GetChildArray().erase(iter);
			}
			else
			{
				++iter;
			}
		}
		// 현재 자기 자식 컴포넌트 배열에 이 자식 컴포넌트를 추가하고
		m_vChildComponent.push_back(TempChild);
		// 자식컴포넌트의 부모 컴포넌트는 내가 된다.
		TempChild->SetParent(this);
	}
	// 아니면 그냥 추가..
	else
	{
		m_vChildComponent.push_back(Child);
		Child->SetParent(this);
	}
}

Component* Component::GetParent()
{
	return m_ParentComponent;
}
std::vector<Component*>& Component::GetChildArray()
{
	return m_vChildComponent;
}
std::vector<Component*> Component::GetChildArray(const char* ComponentID)
{
	vector<Component*> result;
	for (auto& iter : m_vChildComponent)
	{
		if (iter->GetID() == ComponentID)
		{
			result.push_back(iter);
		}
	}
	return result;
}
Component* Component::GetChild(const char* ComponentID)
{
	for (auto& iter : m_vChildComponent)
	{
		if (ComponentID == iter->GetID())
		{
			return iter;
		}
	}
	return nullptr;
}
Component* Component::GetChild(const std::wstring& ComponentName)
{
	for (auto& iter : m_vChildComponent)
	{
		if (ComponentName == iter->GetComponentName())
		{
			return iter;
		}
	}
	return nullptr;
}
Component* Component::GetChild(const size_t index)
{
	if (m_vChildComponent.size() <= index)
	{
		return nullptr;
	}
	return m_vChildComponent[index];
}

void Component::SetParent(Component* Parent)
{
	m_ParentComponent = Parent;
}
