#include "pch.h"
#include "JGUIElementComponent.h"

#include "GUI/JGUIObject/JGUIElement.h"
using namespace std;

void JGUIElementComponent::JGUIAwake()
{
	if (GetOwner() == nullptr) return;
	JGUIComponent::JGUIAwake();
}

void JGUIElementComponent::JGUIStart()
{
	if (GetOwner() == nullptr) return;
	JGUIComponent::JGUIStart();
}

void JGUIElementComponent::JGUIDestroy()
{
	if (GetOwner() == nullptr) return;
	JGUIComponent::JGUIDestroy();
}

void JGUIElementComponent::JGUITick(float tick)
{
	if (GetOwner() == nullptr) return;
	JGUIComponent::JGUITick(tick);
}

void JGUIElementComponent::JGUIChangeParent(JGUIObject* prevParent)
{
	if (GetOwner() == nullptr) return;

	JGUIComponent::JGUIChangeParent(prevParent);
}

JGUIElement* JGUIElementComponent::GetOwner() const
{
	return m_ElementOwner;
}

JGUIWindow* JGUIElementComponent::GetOwnerWindow() const
{
	return GetOwner()->GetOwnerWindow();
}
