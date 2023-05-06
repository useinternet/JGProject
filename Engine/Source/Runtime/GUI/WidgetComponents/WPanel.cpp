#include "PCH/PCH.h"
#include "WPanel.h"
#include "Builder/GUIBuilder.h"
#include "External/imgui/imgui.h"

void WPanel::OnGUIBuild(HGUIBuilder& inBuilder)
{
	inBuilder.PushGenerateNativeGUI(SharedWrap(this), HOnGenerateNativeGUI::Create(SharedWrap(this), [&](const HWidgetContext& widgetContext)
		{
			ImGui::BeginGroup();

			Content.ExecuteIfBound(inBuilder);

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				OnMouseLeftClick.ExecuteIfBound();
			}

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				OnMouseRightClick.ExecuteIfBound();
			}

			ImGui::EndGroup();
		}));
}
