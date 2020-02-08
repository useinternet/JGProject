#include"pch.h"
#include "JGUIInputText.h"
#include "JGUIPanel.h"
#include "JGUIShape.h"
#include "JGUIRectTransform.h"



void JGUIInputText::Awake()
{
	RegisterCollider(JGUI_Component_Colider_Box);
	m_BackGound = CreateJGUIComponent<JGUIPanel>("BackGround");
	m_TextBackGround = CreateJGUIComponent<JGUIPanel>("TextBackGround");
	m_Text = CreateJGUIComponent<JGUIText>("Text");
	m_Flash = CreateJGUIComponent<JGUIRectangle>("Line");


	


}
void JGUIInputText::Start()
{
	m_TextBackGround->SetParent(m_BackGound);
	m_Text->SetParent(m_TextBackGround);
	m_Flash->SetParent(m_TextBackGround);



	float gap = 5.f;
	m_TextBackGround->GetTransform()->SetPosition(gap, gap);
	Setting();


	m_Text->SetText("(none)");
	m_Text->SetColor(JColor(0, 0, 0, 1));
	m_Flash->SetColor(JColor(0.1f, 0.1f, 0.1f, 1.0f));


	auto last_pos = m_Text->GetTextLastPos();
	m_Flash->GetTransform()->SetPosition(last_pos.x, 0);

	m_Flash->SetActive(false);
}
void JGUIInputText::KeyDown(const JGUIKeyDownEvent& e)
{



	m_PressedTick += GlobalLinkData::GetTick();


	if (m_CurrentChar == e.ToString() && m_PressedTick >= 0.5f)
	{
		m_Text->AddText(m_CurrentChar);


	}
	if (m_CurrentChar == "\b" && e.Code == KeyCode::BackSpace && m_PressedTick >= 0.5f)
	{
		m_Text->RemoveBack();
	}
	auto last_pos = m_Text->GetTextLastPos();
	m_Flash->GetTransform()->SetPosition(last_pos.x, 0);
}

void JGUIInputText::KeyUp(const JGUIKeyUpEvent& e)
{
	m_PressedTick = 0.0f;

}

void JGUIInputText::Char(const JGUICharEvent& e)
{
	if (e.str == "\r")
		return;
	if (e.str == "\b")
	{
		m_Text->RemoveBack();
	}
	else
	{
		m_Text->AddText(e.str);
	}


	auto last_pos = m_Text->GetTextLastPos();
	m_Flash->GetTransform()->SetPosition(last_pos.x, 0);
	m_CurrentChar = e.str;
}

void JGUIInputText::FocusEnter(const JGUIFocusEnterEvent& e)
{
	m_Flash->SetActive(true);
	m_FlashTick = 0.0f;
}

void JGUIInputText::OnFocus()
{
	m_FlashTick += GlobalLinkData::GetTick();
	if (m_FlashTick >= m_FlashTime)
	{
		m_FlashTick = 0.0f;
		if (m_Flash->IsActive()) m_Flash->SetActive(false);
		else m_Flash->SetActive(true);
	}
}

void JGUIInputText::FocusExit(const JGUIFocusExitEvent& e)
{
	m_FlashTick = 0.0f;
	m_Flash->SetActive(false);
}

void JGUIInputText::Resize(const JGUIResizeEvent& e)
{
	Setting();

}



void JGUIInputText::Setting()
{
	m_BackGound->SetColor(JColor(0.2f, 0.2f, 0.2f, 1.0f));
	m_TextBackGround->SetColor(JColor(1, 1, 1, 1));

	auto size = GetTransform()->GetSize();
	m_BackGound->GetTransform()->SetSize(size);
	size.x -= 10;
	size.y -= 10;
	m_TextBackGround->GetTransform()->SetSize(size);
	m_Text->SetTextRect(size.x, size.y);

	m_Flash->GetTransform()->SetSize(m_FlashThickness, size.y);
	
}