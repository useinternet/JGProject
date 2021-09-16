#include "pch.h"
#include "ImGuiExternal.h"
#include "JGImGui.h"
#include "Application.h"
#include "Common/Type.h"
#include "Common/DragAndDrop.h"
#include "Class/Asset/Asset.h"

namespace ImGui
{
	void Vector4_OnGUI(const std::string& label, JG::JVector4& v, float label_space)
	{
		float winWidth = ImGui::GetWindowWidth();
		ImGui::Columns(5, 0, false);
		ImGui::AlignTextToFramePadding();

		if (label_space <= 0.0f)
		{
			label_space = ImGui::CalcTextSize(label.c_str()).x;
		}


		if (label_space > 0.0f)
		{
			ImGui::SetColumnWidth(0, label_space + (ImGui::GetStyle().ItemSpacing.x * 2));
		}

		ImGui::Text(label.c_str()); ImGui::SameLine();


		label_space = ImGui::GetColumnWidth();
		float oneColumnWidth = (winWidth - label_space) / 4;
		float btWidth = 0.0f;
		ImGui::NextColumn();



		ImGui::SetColumnWidth(1, oneColumnWidth);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
		ImGui::Button("X"); btWidth = ImGui::GetItemRectSize().x;
		ImGui::PopStyleColor(); ImGui::PopStyleColor(); ImGui::PopStyleColor();
		ImGui::SameLine();  ImGui::SetNextItemWidth(oneColumnWidth - btWidth - ImGui::GetStyle().ItemSpacing.x * 3);
		ImGui::InputFloat(("##_x" + label).c_str(), (float*)&(v.x));
		ImGui::NextColumn();


		ImGui::SetColumnWidth(2, oneColumnWidth);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));
		ImGui::Button("Y"); btWidth = ImGui::GetItemRectSize().x;
		ImGui::PopStyleColor(); ImGui::PopStyleColor(); ImGui::PopStyleColor();
		ImGui::SameLine(); ImGui::SetNextItemWidth(oneColumnWidth - btWidth - ImGui::GetStyle().ItemSpacing.x * 3);
		ImGui::InputFloat(("##_y" + label).c_str(), (float*)&(v.y));
		ImGui::NextColumn();

		ImGui::SetColumnWidth(3, oneColumnWidth);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.5f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.5f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.5f, 1.0f));
		ImGui::Button("Z"); 	btWidth = ImGui::GetItemRectSize().x;
		ImGui::PopStyleColor(); ImGui::PopStyleColor(); ImGui::PopStyleColor();
		ImGui::SameLine(); ImGui::SetNextItemWidth(oneColumnWidth - btWidth - ImGui::GetStyle().ItemSpacing.x * 3);
		ImGui::InputFloat(("##_z" + label).c_str(), (float*)&(v.z));
		ImGui::NextColumn();



		ImGui::SetColumnWidth(4, oneColumnWidth);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		ImGui::Button("W"); 	btWidth = ImGui::GetItemRectSize().x;
		ImGui::PopStyleColor(); ImGui::PopStyleColor(); ImGui::PopStyleColor();
		ImGui::SameLine(); ImGui::SetNextItemWidth(oneColumnWidth - btWidth - ImGui::GetStyle().ItemSpacing.x * 3);
		ImGui::InputFloat(("##_w" + label).c_str(), (float*)&(v.w));


		ImGui::Columns(1);
	}
	void Vector3_OnGUI(const std::string& label, JG::JVector3& v, float label_space)
	{
		float winWidth = ImGui::GetWindowWidth();
		ImGui::Columns(4, 0, false);
		ImGui::AlignTextToFramePadding();
		if (label_space <= 0.0f)
		{
			label_space = ImGui::CalcTextSize(label.c_str()).x;
		}
		if (label_space > 0.0f)
		{ 
			ImGui::SetColumnWidth(0, label_space + (ImGui::GetStyle().ItemSpacing.x * 2));
		}

		ImGui::Text(label.c_str()); ImGui::SameLine();
	

		label_space = ImGui::GetColumnWidth();
		float oneColumnWidth = (winWidth - label_space) / 3;
		float btWidth = 0.0f;
		ImGui::NextColumn();



		ImGui::SetColumnWidth(1, oneColumnWidth);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
		ImGui::Button("X"); btWidth = ImGui::GetItemRectSize().x;
		ImGui::PopStyleColor(); ImGui::PopStyleColor(); ImGui::PopStyleColor();
		ImGui::SameLine();  ImGui::SetNextItemWidth(oneColumnWidth - btWidth - ImGui::GetStyle().ItemSpacing.x * 3);
		ImGui::InputFloat(("##_x" + label).c_str(), (float*)&(v.x));
		ImGui::NextColumn();


		ImGui::SetColumnWidth(2, oneColumnWidth);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));
		ImGui::Button("Y"); btWidth = ImGui::GetItemRectSize().x;
		ImGui::PopStyleColor(); ImGui::PopStyleColor(); ImGui::PopStyleColor();
		ImGui::SameLine(); ImGui::SetNextItemWidth(oneColumnWidth - btWidth - ImGui::GetStyle().ItemSpacing.x * 3);
		ImGui::InputFloat(("##_y" + label).c_str(), (float*)&(v.y));
		ImGui::NextColumn();

		ImGui::SetColumnWidth(3, oneColumnWidth);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.5f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.5f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.5f, 1.0f));
		ImGui::Button("Z"); 	btWidth = ImGui::GetItemRectSize().x;
		ImGui::PopStyleColor(); ImGui::PopStyleColor(); ImGui::PopStyleColor();
		ImGui::SameLine(); ImGui::SetNextItemWidth(oneColumnWidth - btWidth - ImGui::GetStyle().ItemSpacing.x * 3);
		ImGui::InputFloat(("##_z" + label).c_str(), (float*)&(v.z));
		ImGui::Columns(1);

	}
	void Vector2_OnGUI(const std::string& label, JG::JVector2& v, float label_space)
	{
		float winWidth = ImGui::GetWindowWidth();
		ImGui::Columns(3, 0, false);
		ImGui::AlignTextToFramePadding();

		if (label_space <= 0.0f)
		{
			label_space = ImGui::CalcTextSize(label.c_str()).x;
		}

		if (label_space > 0.0f)
		{
			ImGui::SetColumnWidth(0, label_space + (ImGui::GetStyle().ItemSpacing.x * 2));
		}

		ImGui::Text(label.c_str()); ImGui::SameLine();


		label_space = ImGui::GetColumnWidth();
		float oneColumnWidth = (winWidth - label_space) / 2;

		float btWidth = 0.0f;
		ImGui::NextColumn();

		ImGui::SetColumnWidth(1, oneColumnWidth);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
		ImGui::Button("X"); btWidth = ImGui::GetItemRectSize().x;
		ImGui::PopStyleColor(); ImGui::PopStyleColor(); ImGui::PopStyleColor();
		ImGui::SameLine();  ImGui::SetNextItemWidth(oneColumnWidth - btWidth - ImGui::GetStyle().ItemSpacing.x * 3);
		ImGui::InputFloat(("##_x" + label).c_str(), (float*)&(v.x));
		ImGui::NextColumn();


		ImGui::SetColumnWidth(2, oneColumnWidth);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));
		ImGui::Button("Y"); btWidth = ImGui::GetItemRectSize().x;
		ImGui::PopStyleColor(); ImGui::PopStyleColor(); ImGui::PopStyleColor();
		ImGui::SameLine(); ImGui::SetNextItemWidth(oneColumnWidth - btWidth - ImGui::GetStyle().ItemSpacing.x * 3);
		ImGui::InputFloat(("##_y" + label).c_str(), (float*)&(v.y));

		ImGui::Columns(1);
	}
	void Color4_OnGUI(const std::string& label, JG::Color& c, float label_space)
	{
		ImGui::Columns(2, 0, false);
		ImGui::AlignTextToFramePadding();
		if (label_space <= 0.0f)
		{
			label_space = ImGui::CalcTextSize(label.c_str()).x;
		}
		if (label_space > 0.0f)
		{
			ImGui::SetColumnWidth(0, label_space + (ImGui::GetStyle().ItemSpacing.x * 2));
		}

		ImGui::Text(label.c_str()); ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::ColorEdit4(("##" + label).c_str(), (float*)&c);
		ImGui::Columns(1);
	}
	void Color3_OnGUI(const std::string& label, JG::Color& c, float label_space)
	{
		ImGui::Columns(2, 0, false);
		ImGui::AlignTextToFramePadding();
		if (label_space <= 0.0f)
		{
			label_space = ImGui::CalcTextSize(label.c_str()).x;
		}
		if (label_space > 0.0f)
		{
			ImGui::SetColumnWidth(0, label_space + (ImGui::GetStyle().ItemSpacing.x * 2));
		}

		ImGui::Text(label.c_str()); ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::ColorEdit3(("##" + label).c_str(), (float*)&c);
		ImGui::Columns(1);
	}
	void Float_OnGUI(const std::string& label, float& f, float label_space)
	{
		ImGui::Columns(2, 0, false);
		ImGui::AlignTextToFramePadding();
		if (label_space <= 0.0f)
		{
			label_space = ImGui::CalcTextSize(label.c_str()).x;
		}

		if (label_space > 0.0f)
		{
			ImGui::SetColumnWidth(0, label_space + (ImGui::GetStyle().ItemSpacing.x * 2));
		}

		ImGui::Text(label.c_str()); ImGui::SameLine();
		ImGui::NextColumn(); ImGui::SetNextItemWidth(150.0F);
		ImGui::InputFloat(("##" + label).c_str(), &f);
		ImGui::Columns(1);
	}
	void Int_OnGUI(const std::string& label, int& i, float label_space)
	{
		ImGui::Columns(2, 0, false);
		ImGui::AlignTextToFramePadding();
		if (label_space <= 0.0f)
		{
			label_space = ImGui::CalcTextSize(label.c_str()).x;
		}
		if (label_space > 0.0f)
		{
			ImGui::SetColumnWidth(0, label_space + (ImGui::GetStyle().ItemSpacing.x * 2));
		}

		ImGui::Text(label.c_str()); ImGui::SameLine();
		ImGui::NextColumn(); ImGui::SetNextItemWidth(150.0F);
		ImGui::InputInt(("##" + label).c_str(), &i);
		ImGui::Columns(1);
	}
	void Bool_OnGUI(const std::string& label, bool& b, float label_space)
	{
		ImGui::Columns(2, 0, false);
		ImGui::AlignTextToFramePadding();
		if (label_space <= 0.0f)
		{
			label_space = ImGui::CalcTextSize(label.c_str()).x;
		}

		if (label_space > 0.0f)
		{
			ImGui::SetColumnWidth(0, label_space + (ImGui::GetStyle().ItemSpacing.x * 2));
		}

		ImGui::Text(label.c_str()); ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::Checkbox(("##" + label).c_str(), &b);
		ImGui::Columns(1);
	}

	bool AssetField_OnGUI(const std::string& label, const std::string& inputText, JG::EAssetFormat format, std::string& out_path, float label_space )
	{
		bool result = false;
		ImGui::Columns(2, 0, false);
		ImGui::AlignTextToFramePadding();
		if (label_space <= 0.0f)
		{
			label_space = ImGui::CalcTextSize(label.c_str()).x;
		}

		if (label_space > 0.0f)
		{
			ImGui::SetColumnWidth(0, label_space + (ImGui::GetStyle().ItemSpacing.x * 2));
		}
		auto inputT = inputText;
		ImGui::Text(label.c_str()); ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::InputText(("##" + label).c_str(), &inputT[0], ImGuiInputTextFlags_ReadOnly);


		JG::DragAndDropTarget<JG::DDDContentsFile>([&](JG::DDDContentsFile* ddd)
		{
			auto assetFormat = JG::AssetDataBase::GetInstance().GetAssetFormat(ddd->FilePath);
			if (assetFormat == format)
			{
				out_path = ddd->FilePath;
				result = true;
			}
		});

		ImGui::Columns(1);
		return result;
	}











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


		JG::DragAndDropTarget<JG::DDDContentsFile>([&](JG::DDDContentsFile* ddd)
		{
			auto assetFormat = JG::AssetDataBase::GetInstance().GetAssetFormat(ddd->FilePath);
			if (assetFormat == format)
			{
				out_AssetPath = ddd->FilePath;
				result = true;
			}
		});
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
		
		JG::DragAndDropTarget<JG::DDDContentsFile>([&](JG::DDDContentsFile* ddd)
		{
			auto dddContentsFile = (JG::DDDContentsFile*)ddd;

			auto assetFormat = JG::AssetDataBase::GetInstance().GetAssetFormat(dddContentsFile->FilePath);
			if (assetFormat == JG::EAssetFormat::Texture)
			{
				out_Assetpath = dddContentsFile->FilePath;
				result = true;
			}
		});


		//if (ImGui::BeginDragDropTarget() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		//{
		//	auto payLoad = ImGui::GetDragDropPayload();
		//	if (payLoad != nullptr)
		//	{
		//		JG::IDragAndDropData* ddd = (JG::IDragAndDropData*)payLoad->Data;


		//		if (ddd->GetType() == JGTYPE(JG::DDDContentsFile))
		//		{
		//			auto dddContentsFile = (JG::DDDContentsFile*)ddd;

		//			auto assetFormat = JG::AssetDataBase::GetInstance().GetAssetFormat(dddContentsFile->FilePath);
		//			if (assetFormat == JG::EAssetFormat::Texture)
		//			{
		//				out_Assetpath = dddContentsFile->FilePath;
		//				result = true;
		//			}
		//		}
		//	}
		//	ImGui::EndDragDropTarget();
		//}
		return result;
	}
}