#include "pch.h"
#include "ImGuiExternal.h"
#include "JGImGui.h"
#include "Application.h"
#include "Common/Type.h"
#include "Common/DragAndDrop.h"
#include "Class/Asset/Asset.h"
#include "Graphics/Resource.h"

namespace ImGui
{
	enum
	{
		Vector_Element_Int,
		Vector_Element_Uint,
		Vector_Element_Float,
	};

	void Vector_OnGUI(const std::string& label,void* data,  int vector_element_enum, int elementCnt, float columnsWidth, int columOffsetIndex)
	{

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
				ImGui::InputInt(("##" + btText + label).c_str(), &((int*)data)[i]);
				break;
			case Vector_Element_Uint:
				ImGui::InputInt(("##" + btText + label).c_str(), &((int*)data)[i], ImGuiInputTextFlags_CharsDecimal);
				break;
			case Vector_Element_Float:
				ImGui::InputFloat(("##" + btText + label).c_str(), &((float*)data)[i]);
				break;
			}


		
			ImGui::NextColumn();
		}
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

	void Vector4_OnGUI(const std::string& label, JG::JVector4& v, float label_space)
	{
		float winWidth = ImGui::GetWindowWidth();
		float oneColumnWidth = 0.0f;
		float btWidth        = 0.0f;

		Label_OnGUI(5, label, label_space, nullptr, [&]()
		{
			label_space = ImGui::GetColumnWidth();
			oneColumnWidth = (winWidth - label_space) / 4;
			btWidth = 0.0f;
		});
		Vector_OnGUI(label, (float*)(&v), Vector_Element_Float, 4, oneColumnWidth, 1);
		ImGui::Columns(1);
	}
	void Vector3_OnGUI(const std::string& label, JG::JVector3& v, float label_space)
	{

		float winWidth = ImGui::GetWindowWidth();

		float oneColumnWidth = 0.0f;
		float btWidth = 0.0f;
		Label_OnGUI(4, label, label_space, nullptr, [&]()
		{
			label_space = ImGui::GetColumnWidth();
			oneColumnWidth = (winWidth - label_space) / 3;
			btWidth = 0.0f;
		});

		Vector_OnGUI(label, (float*)(&v), Vector_Element_Float, 3, oneColumnWidth, 1);
		ImGui::Columns(1);
	}
	void Vector2_OnGUI(const std::string& label, JG::JVector2& v, float label_space)
	{
		float winWidth = ImGui::GetWindowWidth();
		float oneColumnWidth = 0.0f;
		float btWidth = 0.0f;
		Label_OnGUI(3, label, label_space, nullptr, [&]()
		{
			label_space = ImGui::GetColumnWidth();
			oneColumnWidth = (winWidth - label_space) / 2;
			btWidth = 0.0f;
		});
		Vector_OnGUI(label, (float*)(&v), Vector_Element_Float, 2, oneColumnWidth, 1);
		ImGui::Columns(1);
	}

	void Vector4Int_OnGUI(const std::string& label, JG::JVector4Int& v, float label_space)
	{
		float winWidth = ImGui::GetWindowWidth();
		float oneColumnWidth = 0.0f;
		float btWidth = 0.0f;

		Label_OnGUI(5, label, label_space, nullptr, [&]()
		{
			label_space = ImGui::GetColumnWidth();
			oneColumnWidth = (winWidth - label_space) / 4;
			btWidth = 0.0f;
		});
		Vector_OnGUI(label, (float*)(&v), Vector_Element_Int, 4, oneColumnWidth, 1);
		ImGui::Columns(1);
	}
	void Vector3Int_OnGUI(const std::string& label, JG::JVector3Int& v, float label_space)
	{
		float winWidth = ImGui::GetWindowWidth();

		float oneColumnWidth = 0.0f;
		float btWidth = 0.0f;
		Label_OnGUI(4, label, label_space, nullptr, [&]()
		{
			label_space = ImGui::GetColumnWidth();
			oneColumnWidth = (winWidth - label_space) / 3;
			btWidth = 0.0f;
		});

		Vector_OnGUI(label, (float*)(&v), Vector_Element_Int, 3, oneColumnWidth, 1);
		ImGui::Columns(1);
	}
	void Vector2Int_OnGUI(const std::string& label, JG::JVector2Int& v, float label_space)
	{
		float winWidth = ImGui::GetWindowWidth();
		float oneColumnWidth = 0.0f;
		float btWidth = 0.0f;
		Label_OnGUI(3, label, label_space, nullptr, [&]()
		{
			label_space = ImGui::GetColumnWidth();
			oneColumnWidth = (winWidth - label_space) / 2;
			btWidth = 0.0f;
		});
		Vector_OnGUI(label, (float*)(&v), Vector_Element_Int, 2, oneColumnWidth, 1);
		ImGui::Columns(1);
	}
	void Vector4Uint_OnGUI(const std::string& label, JG::JVector4Uint& v, float label_space)
	{
		float winWidth = ImGui::GetWindowWidth();
		float oneColumnWidth = 0.0f;
		float btWidth = 0.0f;

		Label_OnGUI(5, label, label_space, nullptr, [&]()
		{
			label_space = ImGui::GetColumnWidth();
			oneColumnWidth = (winWidth - label_space) / 4;
			btWidth = 0.0f;
		});
		Vector_OnGUI(label, (float*)(&v), Vector_Element_Uint, 4, oneColumnWidth, 1);
		ImGui::Columns(1);
	}
	void Vector3Uint_OnGUI(const std::string& label, JG::JVector3Uint& v, float label_space)
	{
		float winWidth = ImGui::GetWindowWidth();

		float oneColumnWidth = 0.0f;
		float btWidth = 0.0f;
		Label_OnGUI(4, label, label_space, nullptr, [&]()
		{
			label_space = ImGui::GetColumnWidth();
			oneColumnWidth = (winWidth - label_space) / 3;
			btWidth = 0.0f;
		});

		Vector_OnGUI(label, (float*)(&v), Vector_Element_Uint, 3, oneColumnWidth, 1);
		ImGui::Columns(1);
	}
	void Vector2Uint_OnGUI(const std::string& label, JG::JVector2Uint& v, float label_space)
	{
		float winWidth = ImGui::GetWindowWidth();
		float oneColumnWidth = 0.0f;
		float btWidth = 0.0f;
		Label_OnGUI(3, label, label_space, nullptr, [&]()
		{
			label_space = ImGui::GetColumnWidth();
			oneColumnWidth = (winWidth - label_space) / 2;
			btWidth = 0.0f;
		});
		Vector_OnGUI(label, (float*)(&v), Vector_Element_Uint, 2, oneColumnWidth, 1);
		ImGui::Columns(1);
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
	void Float_OnGUI(const std::string& label, float& f, float label_space)
	{
		Label_OnGUI(2, label, label_space, nullptr, nullptr);
		ImGui::SetNextItemWidth(150.0F);
		ImGui::InputFloat(("##" + label).c_str(), &f);
		ImGui::Columns(1);
	}
	void Float_Slider_OnGUI(const std::string& label, float& f, float min, float max, float label_space)
	{
		Label_OnGUI(2, label, label_space, nullptr, nullptr);
		ImGui::SetNextItemWidth(150.0F);
		ImGui::SliderFloat(("##" + label).c_str(), &f, min, max);
		ImGui::Columns(1);
	}
	void Int_OnGUI(const std::string& label, int& i, float label_space)
	{
		ImGui::SetNextItemWidth(150.0F);
		ImGui::InputInt(("##" + label).c_str(), &i);
		ImGui::Columns(1);
	}
	void Uint_OnGUI(const std::string& label, JG::u32& i, float label_space)
	{
		ImGui::SetNextItemWidth(150.0F);
		ImGui::InputInt(("##" + label).c_str(), (int*)&i, ImGuiInputTextFlags_CharsDecimal);
		ImGui::Columns(1);
	}
	void String_OnGUI(const std::string& label, std::string& str, float label_space)
	{
		Label_OnGUI(2, label, label_space, nullptr, nullptr);
		str.resize(512);
		ImGui::InputText(("##" + label).c_str(), str.data(), 512);
		int len = strlen(str.c_str());
		str = str.substr(0, len);
		ImGui::Columns(1);
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


		return JG::AssetDataBase::GetInstance().LoadOriginAsset(texturePathDic[label]);
	}

}