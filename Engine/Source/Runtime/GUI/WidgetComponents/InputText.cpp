#include "PCH/PCH.h"
#include "InputText.h"
#include "Builder/GUIBuilder.h"
#include "External/imgui/imgui.h"

const PString& WInputText::GetInputText() const
{
	return _inputText;
}

void WInputText::OnGUIBuild(HGUIBuilder& inBuilder)
{
	inBuilder.PushGenerateNativeGUI(SharedWrap(this), POnGenerateNativeGUI::Create<WInputText>(SharedWrap(this), [&](const HWidgetContext& widgetContext)
	{
		PString buff; 	buff.Resize(MaxBufferSize);

		bool bIsDirty = ImGui::InputText("##", buff.GetCStr(), MaxBufferSize, 0, 0, 0);
		if (bIsDirty == true)
		{
			_inputText = buff;
		}
	}));
}