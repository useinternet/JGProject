#pragma once

#include "JGCore.h"
#include "Imgui/imgui.h"

namespace JG
{
	enum class EAssetFormat;
}
namespace ImGui
{
	void OnGui(const char* label, JG::JVector3* v, float label_spacing = 0.0f);
	void OnGui(const char* label, JG::JVector2* v, float label_spacing = 0.0f);
	void OnGui(const char* label, JG::f32* v, float label_spacing = 0.0f);
	void OnGui(const char* label, JG::i32* v, float label_spacing = 0.0f);


	bool AssetField(const JG::String& label, const JG::String& inputText, JG::EAssetFormat format, JG::String& out_AssetPath);
}