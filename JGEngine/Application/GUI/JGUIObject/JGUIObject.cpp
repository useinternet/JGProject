#include "pch.h"
#include "JGUIObject.h"
#include "GUI/JGUIComponent/JGUIRectTransform.h"
#include "GUI/JGUIComponent/JGUICollider.h"
#include "GUI/JGUIComponent/JGUIRenderItem.h"

using namespace std;

void JGUIObject::JGUIDestroy()
{
	Destroy();
	if (m_Collider)
	{
		JGUI::DestroyObject(m_Collider);
	}
	if (m_Transform)
	{
		JGUI::DestroyObject(m_Transform);
	}
}

void JGUIObject::RegisterCollider(JGUIWindow* owner_window, EJGUI_Colider type)
{
	if (m_Collider)
	{
		JGUI::DestroyObject(m_Collider);
	}
	switch(type)
	{
	case JGUI_Collider_Box:
		m_Collider = JGUI::CreateJGUIComponent<JGUIBoxCollider>("BoxCollider", owner_window, 
			JGUI_ComponentFlag_NoChild | JGUI_ComponentFlag_NoParent | JGUI_ComponentFlag_LockCreateFunction);
		break;
	case JGUI_Collider_EmptyBox:
		m_Collider = JGUI::CreateJGUIComponent<JGUIEmptyBoxColider>("EmptyBoxCollider", owner_window,
			JGUI_ComponentFlag_NoChild | JGUI_ComponentFlag_NoParent | JGUI_ComponentFlag_LockCreateFunction);
		break;
	}
	if (m_Transform)
	{
		m_Transform->AttachTransform(m_Collider->GetTransform());
	}
}
void JGUIObject::RegisterTransform(JGUIComponent* owner_com, EJGUI_RectTransform type)
{
	RegisterTransform(owner_com->GetOwnerWindow(), type);
	m_Transform->BindComponent(owner_com);
}
void JGUIObject::RegisterTransform(JGUIWindow* owner_window, EJGUI_RectTransform type)
{
	if (m_Transform)
	{
		JGUI::DestroyObject(m_Transform);
	}
	switch (type)
	{
	case JGUI_RectTransform_Default:
		m_Transform = JGUI::CreateJGUIComponent<JGUIRectTransform>("RectTransform", owner_window,
			JGUI_ComponentFlag_NoChild | JGUI_ComponentFlag_NoParent | JGUI_ComponentFlag_LockCreateFunction);
		break;
	case JGUI_RectTransform_Window:
		m_Transform = JGUI::CreateJGUIComponent<JGUIWinRectTransform>("RectTransform", owner_window,
			JGUI_ComponentFlag_NoChild | JGUI_ComponentFlag_NoParent | JGUI_ComponentFlag_LockCreateFunction);
		break;
	}
	m_Transform->BindWindow(owner_window);
	if (m_Collider)
	{
		m_Transform->AttachTransform(m_Collider->GetTransform());
	}
}
