#include "pch.h"
#include "JGUIForm.h"
#include "GUI/JGUIComponent/JGUIShape.h"
#include "GUI/JGUIComponent/JGUIPanel.h"
#include "GUI/JGUIComponent/JGUIRectTransform.h"
#include "GUI/JGUIComponent/JGUIButton.h"
#include "GUI/JGUIComponent/JGUICollider.h"
#include "GUI/JGUIComponent/JGUITitleBar.h"
#include "GUI/JGUIComponent/JGUIResizeBox.h"
#include "GUI/JGUIComponent/JGUIInputText.h"

#include"RenderEngine.h""
using namespace std;
void JGUIForm::Start()
{
	text = CreateJGUIComponent<JGUIText>("defaultText");
	text->GetTransform()->SetLocalPosition(30, 50);
	text->SetTextRect(800, 550);
	text->SetFontSize(16);
	inputText = CreateJGUIComponent<JGUIInputText>("InputText");
	inputText->GetTransform()->SetSize(500, 25);
	inputText->GetTransform()->SetLocalPosition(50, 200);
	inputText->SetTextSize(40);
}

void JGUIForm::Tick(const JGUITickEvent& e)
{
	text->SetText(to_string(e.fps));
}





static int cnt = 0;
void JGUIForm::KeyUp(const JGUIKeyUpEvent& e)
{
	
	if (e.Code == KeyCode::Home)
	{

		auto window = CreateJGUIWindow<JGUIForm>("TestForm"  + to_string(cnt++));
		window->GetTransform()->SetLocalPosition(200, 100);
		window->GetTransform()->SetSize(300, 300);
	}
	if (e.Code == KeyCode::End)
	{
		SetParent(nullptr);
	}
}




