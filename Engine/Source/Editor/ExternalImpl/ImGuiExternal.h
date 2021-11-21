#pragma once

#include "JGCore.h"
#include "Graphics/GraphicsDefine.h"
#include "Imgui/imgui.h"

namespace JG
{
	enum class EAssetFormat;
	class DDDGameNode;

	class IAsset;
	template<class T>
	class Asset;
}
namespace ImGui
{
	std::string GetUniqueID(const std::string& str, JG::u64 uID = -1);
	bool Vector4_OnGUI(const std::string& label, JG::JVector4& v, float label_space = 0.0f);
	bool Vector3_OnGUI(const std::string& label, JG::JVector3& v, float label_space = 0.0f);
	bool Vector2_OnGUI(const std::string& label, JG::JVector2& v, float label_space = 0.0f);
	bool Vector4Int_OnGUI(const std::string& label, JG::JVector4Int& v, float label_space = 0.0f);
	bool Vector3Int_OnGUI(const std::string& label, JG::JVector3Int& v, float label_space = 0.0f);
	bool Vector2Int_OnGUI(const std::string& label, JG::JVector2Int& v, float label_space = 0.0f);
	bool Vector4Uint_OnGUI(const std::string& label, JG::JVector4Uint& v, float label_space = 0.0f);
	bool Vector3Uint_OnGUI(const std::string& label, JG::JVector3Uint& v, float label_space = 0.0f);
	bool Vector2Uint_OnGUI(const std::string& label, JG::JVector2Uint& v, float label_space = 0.0f);
	void Color4_OnGUI(const std::string& label, JG::Color& c, float label_space = 0.0f);
	void Color3_OnGUI(const std::string& label, JG::Color& c, float label_space = 0.0f);
	bool Float_OnGUI(const std::string& label, float& f, float label_space = 0.0f);
	void Float_Slider_OnGUI(const std::string& label, float& f,float min, float max, float label_space = 0.0f);
	bool Int_OnGUI(const std::string& label, int& i, float label_space = 0.0f);
	bool Uint_OnGUI(const std::string& label, JG::u32& i, float label_space = 0.0f);
	bool String_OnGUI(const std::string& label, std::string& str, float label_space = 0.0f);
	void Bool_OnGUI(const std::string& label, bool& b, float label_space = 0.0f);
	void AssetField_OnGUI(const std::string& label, const std::string& inputText, JG::EAssetFormat format, const std::function<void(const std::string&)>& action, float label_space = 0.0f);
	void AssetField_List_OnGUI(const std::string& label, JG::List<std::string>& inputTextList, JG::EAssetFormat format, 
		const std::function<void(int, const std::string&)>& action, const std::function<void()>& add_action, const std::function<void()>& remove_action, float label_space = 0.0f);


	void Float_List_OnGUI(const std::string& label, JG::List<float>& f_list, float label_space = 0.0f);
	void Int_List_OnGUI(const std::string& label, JG::List<int>& i_list, float label_space = 0.0f);
	void String_List_OnGUI(const std::string& label, JG::List<std::string>& str_list, float label_space = 0.0f);


	JG::SharedPtr<JG::IAsset> Texture_OnGUI(const std::string& label, JG::u64 textureID, float label_space = 0.0f);





	bool ImageButton(JG::u64 ID, JG::TextureID textureID, const JG::JVector2& btSize = JG::JVector2(0, 0),
		const JG::JVector2& uv0 = JG::JVector2(0, 0), const JG::JVector2 uv1 = JG::JVector2(1, 1),
		int framePadding = -1, const JG::Color& bgColor = JG::Color(), const JG::Color& tinColor = JG::Color::White());

	bool InputText(void* id, const JG::String& inputText, JG::String& out_str);
	bool CheckBox(void* id, bool check);
}
namespace JG
{

	template<class T>
	bool DragAndDropSource(const std::function<void(T*)>& action)
	{
		if (std::is_base_of<JG::IDragAndDropData, T>::value == false)
		{
			return false;
		}
		if (ImGui::BeginDragDropSource())
		{
			T ddd;
			action(&ddd);
			List<jbyte> btData;
			ddd.GetData(btData);
			ImGui::SetDragDropPayload(ddd.GetType().GetName().c_str(), btData.data(), btData.size());
			ImGui::EndDragDropSource();
			return true;
		}
		return false;
	}

	template<class T>
	bool DragAndDropTarget(const std::function<void(T*)>& action)
	{
		if (std::is_base_of<JG::IDragAndDropData, T>::value == false)
		{
			return false;
		}
		if (ImGui::BeginDragDropTarget() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			auto payLoad = ImGui::GetDragDropPayload();
			if (payLoad != nullptr)
			{
				T ddd;
				if (ddd.GetType() == JGTYPE(T))
				{
					ddd.SetData(payLoad->Data);
					action(static_cast<T*>(&ddd));
				}
			}

			ImGui::EndDragDropTarget();
			return true;
		}
		return false;
	}
}