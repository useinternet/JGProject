#include "pch.h"
#include "RenderingProfile.h"

namespace JE
{
	void RenderingProfile::OnLoad()
	{


	}
	void RenderingProfile::OnGUI()
	{
		if(ImGui::BeginTabBar((GetName() + "_TabBar").c_str()))
		{
			if (ImGui::BeginTabItem("Time Performance"))
			{
				ImGui::Text("Time Performance");

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("DescriptorAllocator State"))
			{
				ImGui::Columns(3, "testcolums", false);
				ImGui::Text("TotalHeapCount : 0 ");
				ImGui::NextColumn();
				ImGui::Text("TotalHeapBlockCount : 0 ");
				ImGui::NextColumn();
				ImGui::Text("None");
				ImGui::Separator();
				ImGui::Columns(1);


				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("GUIAllocator State"))
			{
				ImGui::Text("GUIAllocator");

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("All Resource States"))
			{
				ImGui::Text("All Resource States");

				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}




	}
	void RenderingProfile::OnEvent(Event& e)
	{


	}
	void RenderingProfile::OnDestroy()
	{


	}
}