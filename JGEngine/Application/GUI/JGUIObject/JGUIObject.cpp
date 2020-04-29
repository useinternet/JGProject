#include "pch.h"
#include "JGUIObject.h"
#include "App/Application.h"
#include "GUI/JGUIObject/JGUIWindow.h"
#include "GUI/JGUIComponent/JGUITransform.h"
#include "GUI/JGUIComponent/JGUICollider.h"

using namespace std;

float JGUIObject::GetTick() const
{
	return GlobalLinkData::GetTick();
}

JGUIObject::JGUIObject( const std::string& name) : m_Name(name)
{
	
}

void JGUIObject::JGUIAwake()
{
	Awake();
}

void JGUIObject::JGUIDestroy()
{
	Destroy();
	for (auto& com : m_Coms)
	{
		JGUI::DestroyComponent(com);
	}
	m_Coms.clear();
}

void JGUIObject::JGUITick(const JGUITickEvent& e)
{
	Tick(e);
	for (int i = 0; i < m_Coms.size(); ++i)
	{
		if (!m_Coms[i]->IsExecuteStart()) m_Coms[i]->JGUIStart();
		if (!m_Coms[i]->IsActive()) continue;
		if (m_Coms[i]->IsDestroying()) continue;
		m_Coms[i]->JGUITick(e.deltaTime);
	}

	while (m_DestroyingComponent.size() != 0)
	{
		auto com = m_DestroyingComponent.front();
		for (auto& iter = m_Coms.begin(); iter < m_Coms.end();)
		{
			if ((*iter) == com)
			{
				JGUI::DestroyComponent(com);
				iter = m_Coms.erase(iter);
				break;
			}
			else ++iter;
		}
		m_DestroyingComponent.pop();
	}
}

void JGUIObject::SetActive(bool active, bool is_hierarchy)
{
	m_Active = active;
	if (!is_hierarchy)
	{
		m_ActiveSelf = active;
	}
	for (auto& com : m_Coms)
	{
		com->SetActive(active);
	}

}

void JGUIObject::DestroyComponent(JGUIComponent* com)
{
	com->m_IsDestroying = true;
	m_DestroyingComponent.push(com);
}

JGUIComponent* JGUIObject::FindComponent(const std::string& name) const
{
	for (auto& com : m_Coms)
	{
		if (com->GetName() == name) return com;
	}
	return nullptr;
}

JGUIComponent* JGUIObject::GetComponent(int n) const
{
	if (m_Coms.size() <= n) return nullptr;
	return m_Coms[n];
}

uint32_t JGUIObject::GetComponentCount() const
{
	return (uint32_t)m_Coms.size();
}

void JGUIObject::RegisterCollider(EJGUI_Collider type)
{
	if (m_Collider)
	{
		DestroyComponent(m_Collider);
	}
	switch(type)
	{
	case JGUI_Collider_Box:
		m_Collider = CreateJGUIComponent<JGUIBoxCollider>();
		break;
	}
}
