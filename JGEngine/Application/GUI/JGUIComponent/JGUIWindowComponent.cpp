#include "pch.h"
#include "JGUIWindowComponent.h"


using namespace std;
void JGUIWindowComponent::JGUIAwake()
{
	if (GetOwner() == nullptr) return;
	JGUIComponent::JGUIAwake();
}

void JGUIWindowComponent::JGUIStart()
{
	if (GetOwner() == nullptr) return;
	JGUIComponent::JGUIStart();
}

void JGUIWindowComponent::JGUIDestroy()
{
	if (GetOwner() == nullptr) return;
	JGUIComponent::JGUIDestroy();
}

void JGUIWindowComponent::JGUITick(float tick)
{
	if (GetOwner() == nullptr) return;
	JGUIComponent::JGUITick(tick);
}

void JGUIWindowComponent::JGUIChangeParent(JGUIObject* prevParent)
{
	if (GetOwner() == nullptr) return;

	JGUIComponent::JGUIChangeParent(prevParent);
}
JGUIWindow* JGUIWindowComponent::GetOwner() const
{
	return m_WindowOwner;
}
