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
	void DragAndDropSource(const std::function<void(T*)>& action)
	{
		if (std::is_base_of<JG::IDragAndDropData, T>::value == false)
		{
			return;
		}
		if (ImGui::BeginDragDropSource())
		{
			T ddd;
			action(&ddd);
			ImGui::SetDragDropPayload(ddd.GetType().GetName().c_str(), &ddd, sizeof(DDDGameNode));
			ImGui::EndDragDropSource();
		}
	}

	template<class T>
	void DragAndDropTarget(const std::function<void(T*)>& action)
	{
		if (std::is_base_of<JG::IDragAndDropData, T>::value == false)
		{
			return;
		}
		if (ImGui::BeginDragDropTarget() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			auto payLoad = ImGui::GetDragDropPayload();
			if (payLoad != nullptr)
			{
				JG::IDragAndDropData* ddd = (JG::IDragAndDropData*)payLoad->Data;

				if (ddd->GetType() == JGTYPE(T))
				{
					action(static_cast<T*>(ddd));
				}
			}

			ImGui::EndDragDropTarget();
		}
	}
}