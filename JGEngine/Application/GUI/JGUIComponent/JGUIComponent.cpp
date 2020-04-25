#include "pch.h"
#include "JGUIComponent.h"
#include "GUI/JGUIObject/JGUIElement.h"
#include "GUI/JGUIObject/JGUIWindow.h"


void JGUIComponent::SetOwner(JGUIElement* element)
{
	m_ElementOwner = element;
	m_WindowOwner  = nullptr;
}

void JGUIComponent::SetOwner(JGUIWindow* win)
{
	m_WindowOwner = win;
	m_ElementOwner = nullptr;
}

JGUITransform* JGUIComponent::GetTransform() const
{
	if (m_ElementOwner) return m_ElementOwner->GetTransform();
	if (m_WindowOwner)  return m_WindowOwner->GetTransform();
	return nullptr;
}
