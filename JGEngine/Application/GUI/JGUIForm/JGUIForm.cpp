#include "pch.h"
#include "JGUIForm.h"
#include "GUI/JGUIComponent/JGUIShape.h"
#include "GUI/JGUIComponent/JGUIPanel.h"
#include "GUI/JGUIComponent/JGUIRectTransform.h"
#include "GUI/JGUIComponent/JGUIButton.h"
#include "GUI/JGUIComponent/JGUICollider.h"
#include "GUI/JGUIComponent/JGUITitleBar.h"
using namespace std;

void TestJGUIForm::Start()
{
	title = CreateJGUIComponent<JGUITitleBar>("titleBar");
	GetPanel()->SetColor(JColor(0.2f, 0.2f, 0.3f, 0.3f));
}

void TestJGUIForm::Resize(const JGUIResizeEvent& e)
{
	if (title)
	{
		title->GetTransform()->SetSize(e.width, 50);
	}
}

void JGUIForm::Start()
{
	title = CreateJGUIComponent<JGUITitleBar>("titleBar");
	text = CreateJGUIComponent<JGUIText>("defaultText");
	text->GetTransform()->SetPosition(100, 100);
	text->SetTextRect(300, 300);
}

void JGUIForm::Tick(const JGUITickEvent& e)
{

}

void JGUIForm::Resize(const JGUIResizeEvent& e)
{
	if (title)
	{
		title->GetTransform()->SetSize(e.width, 50);
	}
}

void JGUIForm::KeyDown(const JGUIKeyDownEvent& e)
{
	
}

void JGUIForm::KeyUp(const JGUIKeyUpEvent& e)
{
	if (e.Code == KeyCode::A)
	{
		static int cnt = 0;
		auto window = CreateJGUIWindow<JGUIForm>("TestForm" + to_string(cnt++), JGUI_WindowFlag_NewLoad);
		window->GetTransform()->SetSize(300, 300);
	}
	if (e.Code == KeyCode::BackSpace)
	{
		text->RemoveBack();

	}
	if (e.Code == KeyCode::Enter)
	{
		text->Insert(3, "Park");
	}
}
void JGUIForm::Char(const JGUICharEvent& e)
{

	if (e.str == "\r" || e.str == "\b") return;
	text->AddText(e.str);
}
