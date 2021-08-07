#include "pch.h"
#include "AssetFinderContextView.h"

namespace JG
{
	void AssetFinderContextView::Initialize(const AssetFinderInitData& data)
	{
		ImGui::OpenPopup(std::to_string(GetType().GetID()).c_str());
	}
	void AssetFinderContextView::Destroy()
	{

	}
	bool AssetFinderContextView::OnGUI()
	{
		if (ImGui::BeginPopupContextWindow(std::to_string(GetType().GetID()).c_str()) == true)
		{
			char buf[256] = { 0, };
			ImGui::InputText("##Finder Serach", buf, 256);
			ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

			ImGui::BeginChild("##AssetList", ImVec2(250.0f, 600.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NoMove);



			
			ImGui::EndChild();
			ImGui::EndPopup();
			return true;
		}
		else
		{
			return false;
		}
	}
}