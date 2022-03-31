#include "pch.h"
#include "MaterialView.h"
#include "UI/UIManager.h"
namespace JG
{
	MaterialView::MaterialView()
	{
		UIManager::GetInstance().RegisterMainMenuItem("Windows/MaterialView", 0, [&]()
		{
			Open();
		}, nullptr);
	}
	void MaterialView::Load()
	{
	}
	void MaterialView::Initialize()
	{
	}
	void MaterialView::OnGUI()
	{
		ImGui::Text("This is Material View");
	}
	void MaterialView::Destroy()
	{
	}
	void MaterialView::OnEvent(IEvent& e)
	{
	}
}