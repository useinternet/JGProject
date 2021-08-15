#include "pch.h"
#include "ImGuiExternal.h"
#include "JGImGui.h"
#include "Application.h"
#include "Common/Type.h"
#include "Common/DragAndDrop.h"
#include "Class/Asset/Asset.h"

namespace ImGui
{
	void OnGui(const char* label, JG::JVector3* v, float label_spacing)
	{
		std::string id = "##" + std::string(label) + "JVector3";


		ImGui::Spacing();
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label); ImGui::SameLine(); ImGui::Dummy(ImVec2(label_spacing, 0.0f)); ImGui::SameLine();
		ImGui::PushItemWidth(100.0f);
		ImGui::Text("X"); ImGui::SameLine(); auto id_x = id + "_X";  ImGui::InputFloat(id_x.c_str(), (float*)&(v->x)); ImGui::SameLine();
		ImGui::Text("Y"); ImGui::SameLine(); auto id_y = id + "_Y";  ImGui::InputFloat(id_y.c_str(), (float*)&(v->y)); ImGui::SameLine();
		ImGui::Text("Z"); ImGui::SameLine(); auto id_z = id + "_Z";  ImGui::InputFloat(id_z.c_str(), (float*)&(v->z));
		ImGui::PopItemWidth();
		ImGui::Spacing();
	}
	void OnGui(const char* label, JG::JVector2* v, float label_spacing)
	{
		std::string id = "##" + std::string(label) + "JVector2";

		ImGui::Spacing();
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label); ImGui::SameLine(); ImGui::Dummy(ImVec2(label_spacing, 0.0f)); ImGui::SameLine();
		ImGui::PushItemWidth(100.0f);
		ImGui::Text("X"); ImGui::SameLine(); auto id_x = id + "_X"; ImGui::InputFloat(id_x.c_str(), (float*)&(v->x)); ImGui::SameLine();
		ImGui::Text("Y"); ImGui::SameLine(); auto id_y = id + "_Y"; ImGui::InputFloat(id_y.c_str(), (float*)&(v->y));
		ImGui::PopItemWidth();
		ImGui::Spacing();
	}

	void OnGui(const char* label, JG::f32* v, float label_spacing )
	{
		std::string id = "##" + std::string(label) + "Float";

		ImGui::Spacing();
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label); ImGui::SameLine(); ImGui::Dummy(ImVec2(label_spacing, 0.0f)); ImGui::SameLine();
		ImGui::PushItemWidth(100.0f);
		ImGui::SameLine(); auto id_x = id + "_Value"; ImGui::InputFloat(id_x.c_str(), v); ImGui::SameLine();
		ImGui::PopItemWidth();
		ImGui::Spacing();
	}
	void OnGui(const char* label, JG::i32* v, float label_spacing)
	{
		std::string id = "##" + std::string(label) + "Int64";

		ImGui::Spacing();
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label); ImGui::SameLine(); ImGui::Dummy(ImVec2(label_spacing, 0.0f)); ImGui::SameLine();
		ImGui::PushItemWidth(100.0f);
		ImGui::SameLine(); auto id_x = id + "_Value"; ImGui::InputInt(id_x.c_str(), (int*)v); ImGui::SameLine();
		ImGui::PopItemWidth();
		ImGui::Spacing();
	}
	bool AssetField(const JG::String& label, const JG::String& inputText, JG::EAssetFormat format, JG::String& out_AssetPath)
	{
		bool result = false;
		auto id		= "##" + label + inputText;
		auto inputT = inputText;
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str()); ImGui::SameLine();
		ImGui::InputText(id.c_str(), &inputT[0], ImGuiInputTextFlags_ReadOnly);
		if (ImGui::BeginDragDropTarget() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			auto payLoad = ImGui::GetDragDropPayload();
			if (payLoad != nullptr)
			{
				JG::IDragAndDropData* ddd = (JG::IDragAndDropData*)payLoad->Data;


				if (ddd->GetType() == JGTYPE(JG::DDDContentsFile))
				{
					auto dddContentsFile = (JG::DDDContentsFile*)ddd;

					auto assetFormat	 = JG::AssetDataBase::GetInstance().GetAssetFormat(dddContentsFile->FilePath);
					if (assetFormat == format)
					{
						out_AssetPath = dddContentsFile->FilePath;
						result = true;
					}
				}
			}
			ImGui::EndDragDropTarget();
		}


		return result;
	}
	bool TextureAssetField(JG::u64 textureID, JG::String& out_Assetpath)
	{
		bool result = false;
		JG::String resourcePath;
		if (textureID != 0)
		{
			ImGui::Image((ImTextureID)textureID, ImVec2(200, 200));
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
			ImGui::Selectable("##DummyTextureSelectable", true, 0, ImVec2(200, 200));
			ImGui::PopStyleColor();
		}
		

		if (ImGui::BeginDragDropTarget() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			auto payLoad = ImGui::GetDragDropPayload();
			if (payLoad != nullptr)
			{
				JG::IDragAndDropData* ddd = (JG::IDragAndDropData*)payLoad->Data;


				if (ddd->GetType() == JGTYPE(JG::DDDContentsFile))
				{
					auto dddContentsFile = (JG::DDDContentsFile*)ddd;

					auto assetFormat = JG::AssetDataBase::GetInstance().GetAssetFormat(dddContentsFile->FilePath);
					if (assetFormat == JG::EAssetFormat::Texture)
					{
						out_Assetpath = dddContentsFile->FilePath;
						result = true;
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
		return result;
	}
}