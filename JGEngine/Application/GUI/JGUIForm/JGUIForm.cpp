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
	text->GetTransform()->SetPosition(0, 50);
	text->SetTextRect(800, 550);
	text->SetFontSize(256);
	inputText = CreateJGUIComponent<JGUIInputText>("InputText");
	inputText->GetTransform()->SetSize(500, 50);


	inputText->GetTransform()->SetPosition(50, 200);
}





static int cnt = 0;
void JGUIForm::KeyUp(const JGUIKeyUpEvent& e)
{

	if (e.Code == KeyCode::Left)
	{

		auto window = CreateJGUIWindow<JGUIForm>("TestForm"  + to_string(cnt++));
		window->GetTransform()->SetPosition(200, 100);
	}
	if (e.Code == KeyCode::Right)
	{
		SetParent(nullptr);
	}

}




