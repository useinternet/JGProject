#include "pch.h"
#include "JGUIForm.h"
#include "GUI/JGUIComponent/JGUIShape.h"
#include "GUI/JGUIComponent/JGUIPanel.h"
#include "GUI/JGUIComponent/JGUIRectTransform.h"
#include "GUI/JGUIComponent/JGUIButton.h"
#include "GUI/JGUIComponent/JGUICollider.h"
#include "GUI/JGUIComponent/JGUITitleBar.h"
using namespace std;
void JGUITestForm::Start()
{
	title = CreateJGUIComponent<JGUITitleBar>("titleBar");

	GetPanel()->SetColor(JColor(0.0f, 0.0f, 0.1f, 1.0f));
}
void JGUITestForm::Resize(const JGUIResizeEvent& e)
{
	if (title)
	{
		title->GetTransform()->SetSize(e.width, 50);

	}

}

void JGUIForm::Start()
{

	title = CreateJGUIComponent<JGUITitleBar>("titleBar");


	bt = CreateJGUIComponent<JGUIButton>("time");
	//bt->SetBtImage(GlobalLinkData::_EngineConfig->InEngine("Fonts/engineFont_0.png"));
	bt->GetTransform()->SetSize(100, 100);
	bt->GetTransform()->SetPosition(500, 200);



	text = CreateJGUIComponent<JGUIText>("txt");
	text->SetFontSize(24);
	text->GetTransform()->SetPivot(0, 0);
	text->GetTransform()->SetPosition(0, 300);
	text->SetTextRect(300, 100);


	debugText = CreateJGUIComponent<JGUIText>("debug");
	debugText->GetTransform()->SetPivot(0, 0);
	debugText->GetTransform()->SetPosition(0, 50);
	debugText->SetText("None");
	debugText->SetTextRect(500, 150);


	debug = CreateJGUIComponent<JGUIPanel>("rect");
	debug->SetColor({ 1.0f,0.0f,0.0f,0.2f });
	debug->GetTransform()->SetPivot(text->GetTransform()->GetPivot());
	debug->GetTransform()->SetPosition(text->GetTransform()->GetPosition());
	debug->GetTransform()->SetSize(text->GetTransform()->GetSize());



	textBox = CreateJGUIComponent<JGUIPanel>("rect");
	textBox->SetColor({ 0.0f,1.0f,0.0f,0.2f });
	textBox->GetTransform()->SetPivot(text->GetTransform()->GetPivot());
	textBox->GetTransform()->SetPosition(text->GetTransform()->GetPosition());
	auto rect = text->GetTextRect();
	textBox->GetTransform()->SetSize(rect.width, rect.height);


}

void JGUIForm::Resize(const JGUIResizeEvent& e)
{
	if (title)
	{
		title->GetTransform()->SetSize(e.width, 50);
	}
	
}

void JGUIForm::Tick(const JGUITickEvent& e)
{
	debugText->SetText("ToTal : " + to_string(e.totalTime) + " FPS : " + to_string(e.fps) +
		" Delta : " + to_string(e.deltaTime));
	debug->GetTransform()->SetPivot(text->GetTransform()->GetPivot());
	debug->GetTransform()->SetPosition(text->GetTransform()->GetPosition());
	debug->GetTransform()->SetSize(text->GetTransform()->GetSize());


	textBox->GetTransform()->SetPivot(text->GetTransform()->GetPivot());
	textBox->GetTransform()->SetPosition(text->GetTransform()->GetPosition());
	auto rect = text->GetTextRect();
	textBox->GetTransform()->SetSize(rect.width, rect.height);
}
void JGUIForm::KeyDown(const JGUIKeyDownEvent& e)
{
	

	
}

void JGUIForm::KeyUp(const JGUIKeyUpEvent& e)
{
	if (e.Code == KeyCode::BackSpace)
	{
		text->RemoveBack();

	}
	if (e.Code == KeyCode::Enter)
	{
		text->Insert(3, "Park");
	}
	if (e.Code == KeyCode::Tab)
	{
		static int count = 0;

		auto form = CreateJGUIWindow<JGUITestForm>("TestForm" + count++, JGUI_WindowFlag_NewLoad);
		form->GetTransform()->SetSize(300, 300);
		form->GetTransform()->SetPosition(100, 50);

	}

}

void JGUIForm::Char(const JGUICharEvent& e)
{
	if (e.str =="\r" || e.str == "\b") return;
	text->AddText(e.str);
}
