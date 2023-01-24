#include "PCH/PCH.h"
#include "Text.h"
#include "External/imgui/imgui.h"


void WText::GenerateImGuiWidgetComponent(const HWidgetContext& inWidgetContext)
{
	PString Value;
	Text.GetValue(Value);
	ImGui::Text(Value.GetCStr());
}
