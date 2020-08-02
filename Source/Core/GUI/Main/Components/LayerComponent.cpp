#include "pch.h"
#include "LayerComponent.h"
#include "Elements/Element.h"

using namespace std;

namespace GUI
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
}