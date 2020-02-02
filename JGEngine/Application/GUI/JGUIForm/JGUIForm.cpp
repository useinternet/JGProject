#include "pch.h"
#include "JGUIForm.h"
#include "GUI/JGUIComponent/JGUIShape.h"
#include "GUI/JGUIComponent/JGUIPanel.h"
#include "GUI/JGUIComponent/JGUIRectTransform.h"
#include "GUI/JGUIComponent/JGUIButton.h"
#include "GUI/JGUIComponent/JGUICollider.h"
#include "GUI/JGUIComponent/JGUITitleBar.h"
#include "GUI/JGUIComponent/JGUIResizeBox.h"
using namespace std;


void JGUIForm::Start()
{
	auto size = GetTransform()->GetSize();
	m_ResizeBox = CreateJGUIComponent<JGUIResizeBox>("ResizeBox");
	float thickness = m_ResizeBox->GetThickness();
	m_Title = CreateJGUIComponent<JGUITitleBar>("titleBar");
	m_Title->GetTransform()->SetPosition(thickness, thickness);







	text = CreateJGUIComponent<JGUIText>("defaultText");
	text->GetTransform()->SetPosition(0, 50);
	text->SetTextRect(800, 550);
}

void JGUIForm::Tick(const JGUITickEvent& e)
{

}

void JGUIForm::Resize(const JGUIResizeEvent& e)
{
	if (m_Title)
	{
		m_Title->GetTransform()->SetSize(e.width, 50);
	}
	if (m_ResizeBox)
	{
		m_ResizeBox->GetTransform()->SetSize(e.width, e.height);
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