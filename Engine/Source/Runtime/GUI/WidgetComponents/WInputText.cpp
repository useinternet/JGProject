#include "PCH/PCH.h"
#include "WInputText.h"
#include "Builder/GUIBuilder.h"
#include "External/imgui/imgui.h"

void WInputText::Construct(const HArguments& inArgs)
{

}

const PString& WInputText::GetInputText() const
{
	return _inputText;
}

void WInputText::OnGUIBuild(HGUIBuilder& inBuilder)
{
	inBuilder.PushGenerateNativeGUI(SharedWrap(this), HOnGenerateNativeGUI::CreateLambda([&](const HWidgetContext& widgetContext)
	{
		PString buff; 	buff.Resize(_maxBufferSize);

		bool bIsDirty = ImGui::InputText("##", buff.GetCStr(), _maxBufferSize, 0, 0, 0);
		if (bIsDirty == true)
		{
			_inputText = buff;
		}
	}));
}