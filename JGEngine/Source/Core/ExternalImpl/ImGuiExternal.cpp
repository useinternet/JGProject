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
	void String_OnGUI(const std::string& label, std::string& str, float label_space)
	{
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

	void AssetField_OnGUI(const std::string& label, const std::string& inputText, JG::EAssetFormat format, const std::function<void(const std::string&)>& action, float label_space )
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
		auto inputT = inputText;
		ImGui::Text(label.c_str()); ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::InputText(("##" + label).c_str(), &inputT[0], ImGuiInputTextFlags_ReadOnly);


		JG::DragAndDropTarget<JG::DDDContentsFile>([&](JG::DDDContentsFile* ddd)
		{
			auto assetFormat = JG::AssetDataBase::GetInstance().GetAssetFormat(ddd->FilePath);
			if (assetFormat == format)
			{
				action(ddd->FilePath);
			}
		});

		ImGui::Columns(1);
	}

	void AssetField_List_OnGUI(const std::string& label, JG::List<std::string>& inputTextList, JG::EAssetFormat format, 
		const std::function<void(int, const std::string&)>& action, const std::function<void()>& add_action, const std::function<void()>& remove_action, float label_space)
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
		auto cnt = inputTextList.size();
		bool isOpenTree = ImGui::TreeNodeEx(("##Tree" + label).c_str(), ImGuiTreeNodeFlags_DefaultOpen);
		ImGui::SameLine();

		ImGui::Text(("Size : " + std::to_string(cnt)).c_str()); ImGui::SameLine();
		if (ImGui::SmallButton("+"))
		{
			inputTextList.push_back("None");
			add_action();
		}ImGui::SameLine();
		if (ImGui::SmallButton("-"))
		{
			if (inputTextList.empty() == false)
			{
				inputTextList.pop_back();
				remove_action();
			}
		}


		if (isOpenTree)
		{
			cnt = inputTextList.size();
			for (int i = 0; i < cnt; ++i)
			{
				ImGui::AlignTextToFramePadding();
				ImGui::Text(("Slot " + std::to_string(i) + " : ").c_str()); ImGui::SameLine();
				ImGui::InputText(("##" + label + std::to_string(i)).c_str(), inputTextList[i].data(), ImGuiInputTextFlags_ReadOnly);
				JG::DragAndDropTarget<JG::DDDContentsFile>([&](JG::DDDContentsFile* ddd)
				{
					auto assetFormat = JG::AssetDataBase::GetInstance().GetAssetFormat(ddd->FilePath);
					if (assetFormat == format)
					{
						action(i, ddd->FilePath);
					}
				});
			}
			ImGui::TreePop();
		}

		ImGui::Columns(1);
	}

	void Float_List_OnGUI(const std::string& label, JG::List<float>& f_list, float label_space)
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
		auto cnt = f_list.size();
		//bool isOpenTree = ImGui::TreeNodeEx(("##Tree" + label).c_str(), ImGuiTreeNodeFlags_DefaultOpen);
		//ImGui::SameLine();

		ImGui::Text(("Size : " + std::to_string(cnt)).c_str()); ImGui::SameLine();

		//if (ImGui::SmallButton("+"))
		//{
		//	f_list.push_back(0.0f);
		//}ImGui::SameLine();
		//if (ImGui::SmallButton("-"))
		//{
		//	if (f_list.empty() == false)
		//	{
		//		f_list.pop_back();
		//	}
		//
		//}
		//if (isOpenTree)
		//{
		//	cnt = f_list.size();
		//	for (int i = 0; i < cnt; ++i)
		//	{
		//		ImGui::AlignTextToFramePadding();
		//		ImGui::Text(("Slot " + std::to_string(i) + " : ").c_str()); ImGui::SameLine();
		//		ImGui::InputFloat(("##" + label + "input").c_str(), &f_list[i]);
		//	}
		//	ImGui::TreePop();
		//}

		ImGui::Columns(1);
	}

	void Int_List_OnGUI(const std::string& label, JG::List<int>& i_list, float label_space)
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

		auto cnt = i_list.size();
		bool isOpenTree = ImGui::TreeNodeEx(("##Tree" + label).c_str(), ImGuiTreeNodeFlags_DefaultOpen);
		ImGui::SameLine();

		ImGui::Text(("Size : " + std::to_string(cnt)).c_str()); ImGui::SameLine();
		if (ImGui::SmallButton("+"))
		{
			i_list.push_back(0);
		}ImGui::SameLine();
		if (ImGui::SmallButton("-"))
		{
			i_list.pop_back();
		}


		if (isOpenTree)
		{
			cnt = i_list.size();
			for (int i = 0; i < cnt; ++i)
			{
				ImGui::AlignTextToFramePadding();
				ImGui::Text(("Slot " + std::to_string(i) + " : ").c_str()); ImGui::SameLine();
				ImGui::InputInt(("##" + label + "input").c_str(), &i_list[i]);
			}
			ImGui::TreePop();
		}

		ImGui::Columns(1);
	}

	void String_List_OnGUI(const std::string& label, JG::List<std::string>& str_list, float label_space)
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

		auto cnt = str_list.size();
		bool isOpenTree = ImGui::TreeNodeEx(("##Tree" + label).c_str(), ImGuiTreeNodeFlags_DefaultOpen);
		ImGui::SameLine();

		ImGui::Text(("Size : " + std::to_string(cnt)).c_str()); ImGui::SameLine();
		if (ImGui::SmallButton("+"))
		{
			str_list.push_back("");
		}ImGui::SameLine();
		if (ImGui::SmallButton("-"))
		{
			str_list.pop_back();
		}


		if (isOpenTree)
		{
			cnt = str_list.size();
			for (int i = 0; i < cnt; ++i)
			{
				JG::String str = str_list[i];
				str.resize(512);
				ImGui::AlignTextToFramePadding();
				ImGui::Text(("Slot " + std::to_string(i) + " : ").c_str()); ImGui::SameLine();
				ImGui::InputText(("##" + label + "input").c_str(), str.data(), 512);
				str_list[i] = str;

			}
			ImGui::TreePop();
		}

		ImGui::Columns(1);
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