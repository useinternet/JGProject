#include "pch.h"
#include "FileViewer.h"

namespace JE
{
	void FileViewer::OnLoad()
	{
	}
	void FileViewer::OnGUI()
	{
		static bool is_init_first = true;
		static char input_char[256];
		ImGui::InputText("##FileViewerSearch", input_char, 256);
		ImGui::SameLine();
		if (ImGui::Button("Search"))
		{
			// °Ë»ö
		}
		ImGui::Separator();
		ImGui::Columns(2);

		if (is_init_first)
		{
			ImGui::SetColumnWidth(0, 200);
			is_init_first = false;
		}
		if (ImGui::BeginChild("Test"))
		{
			if (ImGui::TreeNodeEx("Contents"))
			{
				ImGui::TreePop();
			}
			if (ImGui::TreeNodeEx("Source"))
			{
				ImGui::TreePop();
			}


			ImGui::EndChild();
		
		}


	}
	void FileViewer::OnDestroy()
	{
	}
}