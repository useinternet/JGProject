#pragma once

#include "JGCore.h"
#include "Imgui/imgui.h"

namespace JG
{
	enum class EAssetFormat;
	class DDDGameNode;
}
namespace ImGui
{
	void OnGui(const char* label, JG::JVector3* v, float label_spacing = 0.0f);
	void OnGui(const char* label, JG::JVector2* v, float label_spacing = 0.0f);
	void OnGui(const char* label, JG::f32* v, float label_spacing = 0.0f);
	void OnGui(const char* label, JG::i32* v, float label_spacing = 0.0f);


	bool AssetField(const JG::String& label, const JG::String& inputText, JG::EAssetFormat format, JG::String& out_AssetPath);
	bool TextureAssetField(JG::u64 textureID,  JG::String& out_Assetpath);










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