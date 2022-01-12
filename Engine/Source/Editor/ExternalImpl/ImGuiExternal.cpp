#include "pch.h"
#include "ImGuiExternal.h"
#include "JGImGui.h"
#include "Application.h"
#include "JGImGui.h"
#include "Common/Type.h"
#include "Common/DragAndDrop.h"
#include "Class/Asset/Asset.h"
#include "Graphics/Resource.h"
#include <Imgui/imgui_internal.h>

namespace ImGui
{
	enum
	{
		Vector_Element_Int,
		Vector_Element_Uint,
		Vector_Element_Float,
	};

	bool Vector_OnGUI(const std::string& label,void* data,  int vector_element_enum, int elementCnt, float columnsWidth, int columOffsetIndex)
	{
		bool isDeactive = false;
		if (elementCnt <= 1 || elementCnt >= 5)
		{
			assert("not supported vector type");
		}



		for (int i = 0; i < elementCnt; ++i)
		{
			std::string btText = "";
			ImVec4 btColor;
			switch (i)
			{
			case 0: btText = "X"; btColor = ImVec4(0.5f, 0.0f, 0.0f, 1.0f);  break;
			case 1: btText = "Y"; btColor = ImVec4(0.0f, 0.5f, 0.0f, 1.0f); break;
			case 2: btText = "Z"; btColor = ImVec4(0.0f, 0.0f, 0.5f, 1.0f); break;
			case 3: btText = "W"; btColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); break;
			}
			ImGui::SetColumnWidth(columOffsetIndex + i, columnsWidth);
			ImGui::PushStyleColor(ImGuiCol_Button, btColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, btColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, btColor);
			ImGui::Button(btText.c_str()); float btWidth = ImGui::GetItemRectSize().x;
			ImGui::PopStyleColor(); ImGui::PopStyleColor(); ImGui::PopStyleColor();
			ImGui::SameLine();  ImGui::SetNextItemWidth(columnsWidth - btWidth - ImGui::GetStyle().ItemSpacing.x * 3);

			
			switch (vector_element_enum)
			{
			case Vector_Element_Int:
				isDeactive = ImGui::DragInt(("##" + btText + label).c_str(), &((int*)data)[i]);
				break;
			case Vector_Element_Uint:
				isDeactive = ImGui::DragInt(("##" + btText + label).c_str(), &((int*)data)[i], 1.0f, 0, JG::JG_I32_MAX);
				break;
			case Vector_Element_Float:
				isDeactive = ImGui::DragFloat(("##" + btText + label).c_str(), &((float*)data)[i]);
				break;
			}

			//if (ImGui::IsItemDeactivatedAfterEdit() == true)
			//{
			//	
			//}
			ImGui::NextColumn();
		}

		return isDeactive;
	}


	void Label_OnGUI(int columnsCnt, const std::string& label, float label_space, std::function<void()> preprocess, std::function<void()> postprocess)
	{
		auto columnsID = GetUniqueID(label, JG::JG_U64_MAX - 1);
		ImGui::Columns(columnsCnt, columnsID.c_str(), false);


		if (preprocess)
		{
			preprocess();
		}


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

		if (postprocess)
		{
			postprocess();
		}
		ImGui::NextColumn();
	}

	void Data_OnGUI()
	{

	}







	std::string GetUniqueID(const std::string& str, JG::u64 uID)
	{
		static JG::u64 offsetID = 0;
		if (uID == -1)
		{
			return "##" + str + std::to_string(uID) + std::to_string(offsetID++);
		}
		else
		{
			return "##" + str + std::to_string(uID);
		}
	}

	bool Vector4_OnGUI(const std::string& label, JG::JVector4& v, float label_space)
	{
		bool result = false;
		float winWidth = ImGui::GetWindowWidth();
		float oneColumnWidth = 0.0f;
		float btWidth        = 0.0f;

		Label_OnGUI(5, label, label_space, nullptr, [&]()
		{
			label_space = ImGui::GetColumnWidth();
			oneColumnWidth = (winWidth - label_space) / 4;
			btWidth = 0.0f;
		});
		result = Vector_OnGUI(label, (float*)(&v), Vector_Element_Float, 4, oneColumnWidth, 1);
		ImGui::Columns(1);

		return result;
	}
	bool Vector3_OnGUI(const std::string& label, JG::JVector3& v, float label_space)
	{
		bool result = false;
		float winWidth = ImGui::GetWindowWidth();

		float oneColumnWidth = 0.0f;
		float btWidth = 0.0f;
		Label_OnGUI(4, label, label_space, nullptr, [&]()
		{
			label_space = ImGui::GetColumnWidth();
			oneColumnWidth = (winWidth - label_space) / 3;
			btWidth = 0.0f;
		});

		result = Vector_OnGUI(label, (float*)(&v), Vector_Element_Float, 3, oneColumnWidth, 1);
		ImGui::Columns(1);

		return result;
	}
	bool Vector2_OnGUI(const std::string& label, JG::JVector2& v, float label_space)
	{
		bool result = false;
		float winWidth = ImGui::GetWindowWidth();
		float oneColumnWidth = 0.0f;
		float btWidth = 0.0f;
		Label_OnGUI(3, label, label_space, nullptr, [&]()
		{
			label_space = ImGui::GetColumnWidth();
			oneColumnWidth = (winWidth - label_space) / 2;
			btWidth = 0.0f;
		});
		result = Vector_OnGUI(label, (float*)(&v), Vector_Element_Float, 2, oneColumnWidth, 1);
		ImGui::Columns(1);

		return result;
	}

	bool Vector4Int_OnGUI(const std::string& label, JG::JVector4Int& v, float label_space)
	{
		bool result = false;
		float winWidth = ImGui::GetWindowWidth();
		float oneColumnWidth = 0.0f;
		float btWidth = 0.0f;

		Label_OnGUI(5, label, label_space, nullptr, [&]()
		{
			label_space = ImGui::GetColumnWidth();
			oneColumnWidth = (winWidth - label_space) / 4;
			btWidth = 0.0f;
		});
		result = Vector_OnGUI(label, (float*)(&v), Vector_Element_Int, 4, oneColumnWidth, 1);
		ImGui::Columns(1);
		return result;
	}
	bool Vector3Int_OnGUI(const std::string& label, JG::JVector3Int& v, float label_space)
	{
		bool result = false;
		float winWidth = ImGui::GetWindowWidth();

		float oneColumnWidth = 0.0f;
		float btWidth = 0.0f;
		Label_OnGUI(4, label, label_space, nullptr, [&]()
		{
			label_space = ImGui::GetColumnWidth();
			oneColumnWidth = (winWidth - label_space) / 3;
			btWidth = 0.0f;
		});

		result = Vector_OnGUI(label, (float*)(&v), Vector_Element_Int, 3, oneColumnWidth, 1);
		ImGui::Columns(1);

		return result;
	}
	bool Vector2Int_OnGUI(const std::string& label, JG::JVector2Int& v, float label_space)
	{
		bool result = false;
		float winWidth = ImGui::GetWindowWidth();
		float oneColumnWidth = 0.0f;
		float btWidth = 0.0f;
		Label_OnGUI(3, label, label_space, nullptr, [&]()
		{
			label_space = ImGui::GetColumnWidth();
			oneColumnWidth = (winWidth - label_space) / 2;
			btWidth = 0.0f;
		});
		result = Vector_OnGUI(label, (float*)(&v), Vector_Element_Int, 2, oneColumnWidth, 1);
		ImGui::Columns(1);

		return result;
	}
	bool Vector4Uint_OnGUI(const std::string& label, JG::JVector4Uint& v, float label_space)
	{
		bool result = false;
		float winWidth = ImGui::GetWindowWidth();
		float oneColumnWidth = 0.0f;
		float btWidth = 0.0f;

		Label_OnGUI(5, label, label_space, nullptr, [&]()
		{
			label_space = ImGui::GetColumnWidth();
			oneColumnWidth = (winWidth - label_space) / 4;
			btWidth = 0.0f;
		});
		result = Vector_OnGUI(label, (float*)(&v), Vector_Element_Uint, 4, oneColumnWidth, 1);
		ImGui::Columns(1);
		return result;
	}
	bool Vector3Uint_OnGUI(const std::string& label, JG::JVector3Uint& v, float label_space)
	{
		bool result = false;
		float winWidth = ImGui::GetWindowWidth();

		float oneColumnWidth = 0.0f;
		float btWidth = 0.0f;
		Label_OnGUI(4, label, label_space, nullptr, [&]()
		{
			label_space = ImGui::GetColumnWidth();
			oneColumnWidth = (winWidth - label_space) / 3;
			btWidth = 0.0f;
		});

		result = Vector_OnGUI(label, (float*)(&v), Vector_Element_Uint, 3, oneColumnWidth, 1);
		ImGui::Columns(1);

		return result;
	}
	bool Vector2Uint_OnGUI(const std::string& label, JG::JVector2Uint& v, float label_space)
	{
		bool result = false;

		float winWidth = ImGui::GetWindowWidth();
		float oneColumnWidth = 0.0f;
		float btWidth = 0.0f;
		Label_OnGUI(3, label, label_space, nullptr, [&]()
		{
			label_space = ImGui::GetColumnWidth();
			oneColumnWidth = (winWidth - label_space) / 2;
			btWidth = 0.0f;
		});
		result = Vector_OnGUI(label, (float*)(&v), Vector_Element_Uint, 2, oneColumnWidth, 1);
		ImGui::Columns(1);
		return result;
	}


	void Color4_OnGUI(const std::string& label, JG::Color& c, float label_space)
	{
		Label_OnGUI(2, label, label_space, nullptr, nullptr);
		ImGui::ColorEdit4(("##" + label).c_str(), (float*)&c);
		ImGui::Columns(1);
	}
	void Color3_OnGUI(const std::string& label, JG::Color& c, float label_space)
	{
		Label_OnGUI(2, label, label_space, nullptr, nullptr);
		ImGui::ColorEdit3(("##" + label).c_str(), (float*)&c);
		ImGui::Columns(1);
	}
	bool Float_OnGUI(const std::string& label, float& f, float label_space, float min , float max, float speed )
	{
		Label_OnGUI(2, label, label_space, nullptr, nullptr);
		ImGui::SetNextItemWidth(150.0F);
		ImGui::DragFloat(("##" + label).c_str(), &f, speed, min, max);
		bool result = ImGui::IsItemDeactivatedAfterEdit();
		ImGui::Columns(1);
		return result;
	}
	void Float_Slider_OnGUI(const std::string& label, float& f, float min, float max, float label_space)
	{
		Label_OnGUI(2, label, label_space, nullptr, nullptr);
		ImGui::SetNextItemWidth(150.0F);
		ImGui::SliderFloat(("##" + label).c_str(), &f, min, max);
		ImGui::Columns(1);
	}
	bool Int_OnGUI(const std::string& label, int& i, float label_space, int min, int max, float speed)
	{
		Label_OnGUI(2, label, label_space, nullptr, nullptr);
		ImGui::SetNextItemWidth(150.0F);
		ImGui::DragInt(("##" + label).c_str(), &i, speed, min, max);
		bool result = ImGui::IsItemDeactivatedAfterEdit();
		ImGui::Columns(1);
		return result;
	}
	bool Uint_OnGUI(const std::string& label, JG::u32& i, float label_space, JG::u32 min, JG::u32 max, float speed)
	{
		Label_OnGUI(2, label, label_space, nullptr, nullptr);
		ImGui::SetNextItemWidth(150.0F);
		ImGui::DragInt(("##" + label).c_str(), (int*)&i, speed, min, max);
		bool result = ImGui::IsItemDeactivatedAfterEdit();
		ImGui::Columns(1);
		return result;
	}
	bool String_OnGUI(const std::string& label, std::string& str, float label_space)
	{
		Label_OnGUI(2, label, label_space, nullptr, nullptr);
		str.resize(512);
		ImGui::InputText(("##" + label).c_str(), str.data(), 512);
		int len = strlen(str.c_str());
		str = str.substr(0, len);
		bool result = ImGui::IsItemDeactivatedAfterEdit();
		ImGui::Columns(1);
		return result;

	}
	void Bool_OnGUI(const std::string& label, bool& b, float label_space)
	{
		Label_OnGUI(2, label, label_space, nullptr, nullptr);
		ImGui::Checkbox(("##" + label).c_str(), &b);
		ImGui::Columns(1);
	}

	void AssetField_OnGUI(const std::string& label, const std::string& inputText, JG::EAssetFormat format, const std::function<void(const std::string&)>& action, float label_space )
	{
		Label_OnGUI(2, label, label_space, nullptr, nullptr);

		auto inputT = inputText;
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
		Label_OnGUI(2, label, label_space, nullptr, nullptr);


		int  idOffset = 0;
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
				auto inputID = GetUniqueID(label, idOffset++);
				ImGui::AlignTextToFramePadding();
				ImGui::Text(("Slot " + std::to_string(i) + " : ").c_str()); ImGui::SameLine();
				ImGui::InputText(inputID.c_str(), inputTextList[i].data(), ImGuiInputTextFlags_ReadOnly);
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
		Label_OnGUI(2, label, label_space, nullptr, nullptr);
		int idOffset = 0;
		int cnt = f_list.size();
		bool isOpenTree = ImGui::TreeNodeEx(("##Tree" + label).c_str(), ImGuiTreeNodeFlags_DefaultOpen);
		ImGui::SameLine();

		ImGui::Text(("Size : " + std::to_string(cnt)).c_str()); ImGui::SameLine();
		if (ImGui::SmallButton("+"))
		{
			f_list.push_back(0.0f);
		}ImGui::SameLine();
		if (ImGui::SmallButton("-"))
		{
			if (f_list.empty() == false)
			{
				f_list.pop_back();
			}
		
		}
		if (isOpenTree)
		{
			cnt = f_list.size();
			for (int i = 0; i < cnt; ++i)
			{
				auto inputID = GetUniqueID(label, idOffset++);
				ImGui::AlignTextToFramePadding();
				ImGui::Text(("Slot " + std::to_string(i) + " : ").c_str()); ImGui::SameLine();
				ImGui::InputFloat(inputID.c_str(), &f_list[i]);
			}
			ImGui::TreePop();
		}



		ImGui::Columns(1);


	
	}

	void Int_List_OnGUI(const std::string& label, JG::List<int>& i_list, float label_space)
	{
		Label_OnGUI(2, label, label_space, nullptr, nullptr);
		int idOffset = 0;
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
				auto inputID = GetUniqueID(label, idOffset++);
				ImGui::AlignTextToFramePadding();
				ImGui::Text(("Slot " + std::to_string(i) + " : ").c_str()); ImGui::SameLine();
				ImGui::InputInt(inputID.c_str(), &i_list[i]);
			}
			ImGui::TreePop();
		}

		ImGui::Columns(1);
	}

	void String_List_OnGUI(const std::string& label, JG::List<std::string>& str_list, float label_space)
	{
		Label_OnGUI(2, label, label_space, nullptr, nullptr);
		int idOffset = 0;
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
				auto inputID = GetUniqueID(label, idOffset++);
				str_list[i].resize(512);
				ImGui::AlignTextToFramePadding();
				ImGui::Text(("Slot " + std::to_string(i) + " : ").c_str()); ImGui::SameLine();
				ImGui::InputText(inputID.c_str(), str_list[i].data(), 512);
				int len = strlen(str_list[i].c_str());
				str_list[i] = str_list[i].substr(0, len);

			}
			ImGui::TreePop();
		}

		ImGui::Columns(1);
	}

	JG::SharedPtr<JG::IAsset> Texture_OnGUI(const std::string& label, JG::u64 textureID, float label_space)
	{
		static JG::Dictionary<std::string, std::string> texturePathDic;
		Label_OnGUI(2, label, label_space, nullptr, nullptr);

		if (textureID == 0)
		{
			auto nullT = JG::ITexture::NullTexture();
			if (nullT && nullT->IsValid())
			{
				textureID = JG::JGImGui::GetInstance().ConvertImGuiTextureID(nullT->GetTextureID());
			}
		}

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
				texturePathDic[label] = ddd->FilePath;
				
			}
		});
		ImGui::Columns(1);

		return JG::AssetDataBase::GetInstance().LoadOriginAsset(texturePathDic[label]);
	}

	bool ImageButton(JG::u64 ID, JG::TextureID textureID, const JG::JVector2& btSize, const JG::JVector2& uv0, const JG::JVector2 uv1, int framePadding, const JG::Color& bgColor, const JG::Color& tinColor)
	{
		ImTextureID imTextureID = (ImTextureID)JG::JGImGui::GetInstance().ConvertImGuiTextureID(textureID);
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		if (window->SkipItems)
			return false;

		PushID((const void*)(textureID + (JG::u64)ID));
		const ImGuiID im_id = window->GetID("#image");
		PopID();

		const ImVec2 padding = (framePadding >= 0) ? ImVec2((float)framePadding, (float)framePadding) : g.Style.FramePadding;
		return ImageButtonEx(im_id, imTextureID, ImVec2(btSize.x, btSize.y), ImVec2(uv0.x, uv0.y), ImVec2(uv1.x, uv1.y), padding, ImVec4(bgColor.R, bgColor.G, bgColor.B, bgColor.A), ImVec4(tinColor.R, tinColor.G, tinColor.B, tinColor.A));
	}
	bool InputText(void* id, const JG::String& inputText, JG::String& out_str)
	{
		auto id_str = GetUniqueID("inputText_", (JG::u64)id);

		std::string text = inputText;
		text.resize(512);

		if (ImGui::InputText(id_str.c_str(), text.data(), 512) == true)
		{
			int len = strlen(text.c_str());
			out_str = text.substr(0, len);
		}

		bool result = ImGui::IsItemDeactivatedAfterEdit();
		return result;
	}

	bool CheckBox(void* id, bool check)
	{
		bool result = check;
		auto id_str = GetUniqueID("CheckBox_", (JG::u64)id);
		if (ImGui::Checkbox(id_str.c_str(), &check) == true)
		{
			result = !result;
		}
		return result;
	}




}