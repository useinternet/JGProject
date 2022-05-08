#include "pch.h"
#include "EditorGUIBuilder.h"
#include "Imgui/imgui.h"


namespace JG
{
	void EditorGUIBuilder::SetID(u64 id)
	{
		mID = id;
	}

	void EditorGUIBuilder::Label(const String& text)
	{
		mCommandQueue.push(CreateSharedPtr<GUICommand_Label>(
			[this](IGUICommand* command)
		{
			GUICommand_Label* command_label = GUICommand_Label::Cast(command);
			if (command_label == nullptr)
			{
				return;
			}

			ImGui::Text(command_label->Data.Label.c_str());
		}, GUICommandData_Label(text)));
	}

	void EditorGUIBuilder::Execute()
	{
		if (mCommandQueue.empty() == true)
		{
			return;
		}



		ImGui::PushID(mID);
		while (mCommandQueue.empty() == false)
		{
			IGUICommand* command = mCommandQueue.front().get();
			command->Execute();
			mCommandQueue.pop();
		}
		ImGui::PopID();

		
	}
}
