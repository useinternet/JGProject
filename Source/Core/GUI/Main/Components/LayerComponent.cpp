#include "pch.h"
#include "LayerComponent.h"
#include "Elements/Element.h"

using namespace std;

namespace JGUI
{
	void LayerComponent::Awake()
	{
		GetOwner()->m_LayerComponent = this;
	}
	void LayerComponent::Tick()
	{
		m_LayerOffset = 0;
	}
	void LayerComponent::Destroy()
	{
		GetOwner()->m_LayerComponent = nullptr;
	}
	uint64_t LayerComponent::GetLayer() const
	{
		if (GetOwner()->GetFlags() & ElementFlag_TopMost) return GUIDraw_Priority_TopMostElement;
		else return m_Layer;
	}
	uint64_t LayerComponent::IssueLayer()
	{
		if (GetOwner()->GetFlags() & ElementFlag_TopMost) return GUIDraw_Priority_TopMostElement;
		else return m_Layer + m_LayerOffset++;
	}
}