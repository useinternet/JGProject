#include "pch.h"
#include "PrefabView.h"

namespace JG
{
	PrefabView::PrefabView()
	{
		DisableUniqueView();
	}

	void PrefabView::Load()
	{
	}

	void PrefabView::Initialize()
	{
	}

	void PrefabView::OnGUI()
	{
		ImGui::Text("This is PrefabView");
	}

	void PrefabView::Destroy()
	{
	}

	void PrefabView::OnEvent(IEvent& e)
	{
	}

	void PrefabView::SetMaterial(const String& path)
	{
	}

	void PrefabView::SetMesh(const String& path)
	{
	}


}


