#include "pch.h"
#include "Component.h"
#include "Elements/Element.h"
#include "Windows/Window.h"
using namespace std;


namespace JGUI
{
	Window* Component::GetOwnerWindow() const
	{
		return m_Owner->GetOwnerWindow();
	}
	Transform* Component::GetTransform() const
	{
		return m_Owner->GetTransform();
	}
}

