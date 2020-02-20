#include "pch.h"
#include "JGUIForm.h"
#include "GUI/JGUIComponent/JGUIShape.h"
#include "GUI/JGUIComponent/JGUIRectTransform.h"
#include "GUI/JGUIComponent/JGUICollider.h"
#include "GUI/JGUIComponent/JGUITitleBar.h"
#include "GUI/JGUIComponent/JGUIResizeBox.h"
#include "GUI/JGUIComponent/JGUIInputText.h"
#include "GUI/JGUIComponent/JGUIMenu.h"
#include "GUI/JGUIComponent/JGUIMenuItem.h"
#include "GUI/JGUIComponent/JGUIText.h"
#include <filesystem>
using namespace std;
void JGUIForm::Start()
{
	
	m_ResizeBox = CreateJGUIComponent<JGUIResizeBox>("ResizeBox");
	m_Title = CreateJGUIComponent<JGUITitleBar>("TitleBar");
	m_Border = CreateJGUIComponent<JGUIEmptyRectangle>("BorderRectangle");
	m_Menu = CreateJGUIComponent<JGUIMenu>("JGUIMenu");


	//
	m_ResizeBox->GetTransform()->SetSize(25, 25);
	m_Border->GetTransform()->SetSize(GetTransform()->GetSize());
	//
	m_Menu->GetTransform()->SetLocalPosition(0.0f, m_Title->GetTransform()->GetSize().y + 1.5f);
	m_Menu->GetTransform()->SetSize(GetTransform()->GetSize().x, 20);


	//
	m_Border->SetPriority(1);
	m_Border->SetColor(JColor(0, 0, 0, 1));
	m_Border->SetThickness(1.0f);
	if (m_Border)
	{
		m_Title->GetTransform()->SetLocalPosition(0.0f, 1.5f);
	}

	// TestCode
	JGUIMenuItemDesc desc;
	//desc.
	auto item1 = m_Menu->AddMenuItem(desc);
	auto sub_item1 = item1->AddMenuItem(desc);
	auto sub_item2 = item1->AddMenuItem(desc);
	auto sub_item3 = item1->AddMenuItem(desc);
	
	sub_item2->AddMenuItem(desc);
	auto sub_sub_item = sub_item2->AddMenuItem(desc);
	sub_sub_item->AddMenuItem(desc);



	auto item2 = m_Menu->AddMenuItem(desc);
	item2->AddMenuItem(desc);
    item2->AddMenuItem(desc)->AddMenuItem(desc);
	item2->AddMenuItem(desc);


	m_Txt = CreateJGUIComponent<JGUIText>("testTxt");
	m_Txt->SetText("TestText\ntestText1\ntestTEXT2");
	m_Txt->GetTransform()->SetLocalPosition(300, 200);
	m_Txt->GetTransform()->SetSize(200, 200);
	m_Txt->SetTextFlag(JGUI_Text_Flag_Border);
}

void JGUIForm::Tick(const JGUITickEvent& e)
{
	
}

void JGUIForm::Resize(const JGUIResizeEvent& e)
{
	if (m_Menu)
	{
		m_Menu->GetTransform()->SetSize(e.width, 20);
	}
	if (m_Title)
	{
		m_Title->GetTransform()->SetSize(e.width, 20);
	}
	if (m_ResizeBox)
	{
		m_ResizeBox->PositionAdjustment();
	}
	if (m_Border)
	{
		m_Border->GetTransform()->SetSize(GetTransform()->GetSize());
	}
}





static int cnt = 0;
void JGUIForm::KeyUp(const JGUIKeyUpEvent& e)
{
	if (e.Code == KeyCode::A)
	{
		m_Txt->SetHAlign(JGUI_Text_HAlignment_Left);
	}
	if (e.Code == KeyCode::S)
	{
		m_Txt->SetHAlign(JGUI_Text_HAlignment_Center);
	}
	if (e.Code == KeyCode::D)
	{
		m_Txt->SetHAlign(JGUI_Text_HAlignment_Right);
	}
	if (e.Code == KeyCode::Q)
	{
		m_Txt->SetVAlign(JGUI_Text_VAlignment_Top);
	}
	if (e.Code == KeyCode::W)
	{
		m_Txt->SetVAlign(JGUI_Text_VAlignment_Center);
	}
	if (e.Code == KeyCode::E)
	{
		m_Txt->SetVAlign(JGUI_Text_VAlignment_Bottom);
	}
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




