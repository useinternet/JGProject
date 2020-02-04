#include "pch.h"
#include "JGUIForm.h"
#include "GUI/JGUIComponent/JGUIShape.h"
#include "GUI/JGUIComponent/JGUIPanel.h"
#include "GUI/JGUIComponent/JGUIRectTransform.h"
#include "GUI/JGUIComponent/JGUIButton.h"
#include "GUI/JGUIComponent/JGUICollider.h"
#include "GUI/JGUIComponent/JGUITitleBar.h"
#include "GUI/JGUIComponent/JGUIResizeBox.h"


#include"RenderEngine.h""
using namespace std;
void JGUIForm::Start()
{
	auto size = GetTransform()->GetSize();
	m_ResizeBox = CreateJGUIComponent<JGUIResizeBox>("ResizeBox");
	float thickness = m_ResizeBox->GetThickness();
	m_Title = CreateJGUIComponent<JGUITitleBar>("titleBar");
	m_Title->GetTransform()->SetPosition(thickness *2, thickness*2);



	text = CreateJGUIComponent<JGUIText>("defaultText");
	text->GetTransform()->SetPosition(0, 50);
	text->SetTextRect(800, 550);
}
static JGUIWindow* ttt = nullptr;
void JGUIForm::Tick(const JGUITickEvent& e)
{
	if (ttt)
	{
		auto size = ttt->GetTransform()->GetSize();
		auto pos = ttt->GetTransform()->GetPosition();
		auto pivot = ttt->GetTransform()->GetPivot();

		std::string str;
		str = "pos : " + to_string(pos.x) + " , " + to_string(pos.y) + "   size : " +
			to_string(size.x) + " , " + to_string(size.y) + "    pivot : " +
			to_string(pivot.x) + " , " + to_string(pivot.y);
		text->SetText(str);
	}

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
	if (e.Code == KeyCode::Up)
	{
		ttt->GetTransform()->OffsetPosition(0, -0.3f);
	}
	if (e.Code == KeyCode::Down)
	{
		ttt->GetTransform()->OffsetPosition(0, 0.3f);
	}
	if (e.Code == KeyCode::Right)
	{
		ttt->GetTransform()->OffsetPosition(0.3f, 0);
	}
	if (e.Code == KeyCode::Left)
	{
		ttt->GetTransform()->OffsetPosition(-0.3f, 0);
	}
}

void JGUIForm::KeyUp(const JGUIKeyUpEvent& e)
{
	static int cnt = 0;
	if (e.Code == KeyCode::A)
	{
		auto window =  CreateJGUIWindow<JGUIForm>("TestForm" + to_string(cnt++), JGUI_WindowFlag_NewLoad);
	}
	if (e.Code == KeyCode::B)
	{
		ttt = CreateJGUIWindow<JGUIForm>("TestForm" + to_string(cnt++));
	}
	if (e.Code == KeyCode::BackSpace)
	{
		text->RemoveBack();
	}
	if (e.Code == KeyCode::Enter)
	{

	}
}
void JGUIForm::Char(const JGUICharEvent& e)
{

	if (e.str == "\r" || e.str == "\b") return;
	text->AddText(e.str);
}