#include "pch.h"
#include "JGScriptEditor.h"
#include "Application.h"

namespace JG
{
	JGScriptEditor::JGScriptEditor(const JVector2& editorSize, bool isChildWindow , const JVector2& pixelScale)
	{
		mEditorSize		= editorSize;
		mIsChildWindow  = isChildWindow;


		const String& rootPath = Application::GetAssetPath();
		mZepEditor = CreateUniquePtr<Zep::ZepEditor_ImGui>(Zep::ZepPath(rootPath), Zep::NVec2f(pixelScale.x, pixelScale.y));

		auto& display = mZepEditor->GetEditor().GetDisplay();
		auto pImFont = ImGui::GetIO().Fonts[0].Fonts[0];
		auto pixelHeight = pImFont->FontSize;

		display.SetFont(Zep::ZepTextType::UI, std::make_shared<Zep::ZepFont_ImGui>(display, pImFont, int(pixelHeight)));
		display.SetFont(Zep::ZepTextType::Text, std::make_shared<Zep::ZepFont_ImGui>(display, pImFont, int(pixelHeight)));
		display.SetFont(Zep::ZepTextType::Heading1, std::make_shared<Zep::ZepFont_ImGui>(display, pImFont, int(pixelHeight * 1.5)));
		display.SetFont(Zep::ZepTextType::Heading2, std::make_shared<Zep::ZepFont_ImGui>(display, pImFont, int(pixelHeight * 1.25)));
		display.SetFont(Zep::ZepTextType::Heading3, std::make_shared<Zep::ZepFont_ImGui>(display, pImFont, int(pixelHeight * 1.125)));
	}

	void JGScriptEditor::Load(const String& fileName, const String& text)
	{
		if (mZepEditor == nullptr)
		{
			return;
		}
		
		mZepEditor->InitWithText(fileName, text);
	}

	void JGScriptEditor::LoadFile(const String& filePath)
	{
		if (mZepEditor == nullptr)
		{
			return;
		}
		mZepEditor->InitWithFileOrDir(filePath);
	}

	void JGScriptEditor::OnGUI()
	{
		if (mZepEditor == nullptr)
		{
			return;
		}
		
		ImGui::SetNextWindowSize(ImVec2(mEditorSize.x, mEditorSize.y), ImGuiCond_FirstUseEver);
		if (mIsChildWindow = false)
		{
			bool show;
			ImGui::Begin("Zep", &show, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar);
		}
		else
		{
			ImGui::BeginChild("Zep", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar);
		}


		auto min = ImGui::GetCursorScreenPos();
		auto max = ImGui::GetContentRegionAvail();
		if (max.x <= 0)
			max.x = 1;
		if (max.y <= 0)
			max.y = 1;
		ImGui::InvisibleButton("ZepContainer", max);

		// Fill the window
		max.x = min.x + max.x;
		max.y = min.y + max.y;

		mZepEditor->SetDisplayRegion(Zep::NVec2f(min.x, min.y), Zep::NVec2f(max.x, max.y));
		mZepEditor->Display();
		bool zep_focused = ImGui::IsWindowFocused();
		if (zep_focused)
		{
			mZepEditor->HandleInput();
		}

		// TODO: A Better solution for this; I think the audio graph is creating a new window and stealing focus
		static int focus_count = 0;
		if (focus_count++ < 2)
		{
			ImGui::SetWindowFocus();
		}

		if (mIsChildWindow == false)
		{
			ImGui::End();
		}
		else
		{
			ImGui::EndChild();
		}

		mZepEditor->GetEditor().RefreshRequired();
	}

	void JGScriptEditor::Destroy()
	{
		mZepEditor.reset();
		mZepEditor = nullptr;
	}

}