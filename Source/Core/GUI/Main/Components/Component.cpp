#include "pch.h"
#include "Component.h"
#include "Elements/Element.h"
#include "Windows/Window.h"
using namespace std;


namespace GUI
{
	Window* Component::GetOwnerWindow() const
	{
		return m_Owner->GetOwnerWindow();
	}
}

