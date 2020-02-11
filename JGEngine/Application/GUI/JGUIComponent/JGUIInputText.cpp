#include"pch.h"
#include "JGUIInputText.h"
#include "JGUIPanel.h"
#include "JGUIShape.h"
#include "JGUIRectTransform.h"
#include "GUI/JGUIObject/JGUIWindow.h"


void JGUIInputText::Awake()
{
	RegisterCollider(GetOwnerWindow(), JGUI_Collider_Box);
	m_TextBackGround = CreateJGUIComponent<JGUIPanel>("TextBackGround");
	m_Text = CreateJGUIComponent<JGUIText>("Text");
	m_SelectedBox = CreateJGUIComponent<JGUIRectangle>("SelectedBox");
	m_Flash = CreateJGUIComponent<JGUIRectangle>("Line");

}
void JGUIInputText::Start()
{
	m_SelectedBox->SetParent(m_TextBackGround);
	m_Text->SetParent(m_TextBackGround);
	m_Flash->SetParent(m_TextBackGround);
	



	Setting();

	m_Text->SetText("(ne)");
	m_CurrentFlashTextIndex = m_Text->Length() - 1;
	ReviseFlashPosByChar();



	m_Text->SetColor(JColor(0, 0, 0, 1));
	m_Flash->SetColor(JColor(0.0f, 0.0f, 0.0f, 1.0f));
	m_SelectedBox->SetColor(JColor(0.0f, 0.0f, 1.0f, 1.0f));
	m_Flash->SetActive(false);
	m_SelectedBox->SetActive(false);
}
void JGUIInputText::MouseBtDown(const JGUIKeyDownEvent& e)
{
	if (e.Code == KeyCode::LeftMouseButton)
	{
		if (!m_Selected)
		{
			m_SelectedEndTextIndex = -2;
			m_SelectedStartTextIndex = INT_MAX;
		}
		m_Selected = true;


		ReviseFlashPosByMousePos();

		//ENGINE_LOG_INFO("Start : {0},  End : {1}", m_SelectedStartTextIndex, m_SelectedEndTextIndex);
		if (m_SelectedStartTextIndex == m_SelectedEndTextIndex)
		{

			m_SelectedBox->SetActive(false);
		}
		else
		{
		
			m_SelectedBox->SetActive(true);
			auto pos = m_SelectedBox->GetTransform()->GetLocalPosition();
			auto size = m_SelectedBox->GetTransform()->GetSize();

			auto start_pos = m_Text->GetTextLastPos(m_SelectedStartTextIndex);
			auto end_pos = m_Text->GetTextLastPos(m_SelectedEndTextIndex);
			//ENGINE_LOG_INFO("Pos :]] Start : {0},  End : {1}", start_pos.x, end_pos.x);
			size.x = end_pos.x - start_pos.x;
			pos.x = start_pos.x;

			m_SelectedBox->GetTransform()->SetLocalPosition(pos);
			m_SelectedBox->GetTransform()->SetSize(size);

		}

	}
}
void JGUIInputText::MouseBtUp(const JGUIKeyUpEvent& e)
{
	m_Selected = false;
}
void JGUIInputText::KeyDown(const JGUIKeyDownEvent& e)
{
	
	m_PressedTick += GlobalLinkData::GetTick();
	// 키 입력
	if (!m_FlashMove)
	{
		if (m_CurrentChar != "" && m_PressedTick >= 0.25f)
		{
			InputText(m_CurrentChar);
			m_PressedTick = 0.0f;
		}
		if (m_CurrentChar == "\b" && e.Code == KeyCode::BackSpace && m_PressedTick >= 0.25f)
		{
			RemoveText();
			m_PressedTick = 0.0f;
		}
		if (e.Code == KeyCode::Right || e.Code == KeyCode::Left)
		{
			m_FlashMove = true;
			FlashMove(e);
		}
	}
	else
	{
		if (m_PressedTick >= 0.05f)
		{
			m_PressedTick = 0.0f;
			FlashMove(e);
		}

	}
}

void JGUIInputText::KeyUp(const JGUIKeyUpEvent& e)
{
	Reset();

}

void JGUIInputText::Char(const JGUICharEvent& e)
{
	if (e.str == "\r")
		return;
	if (e.str == "\b") RemoveText();
	else InputText(e.str);
	m_CurrentChar = e.str;
}

void JGUIInputText::FocusEnter(const JGUIFocusEnterEvent& e)
{
	m_Flash->SetActive(true);
	m_FlashTick = 0.0f;
	//ReviseFlashPosByMousePos();
}

void JGUIInputText::OnFocus()
{
	if (m_FlashMove)
	{
		m_Flash->SetActive(true);
		m_FlashTick = 0.0f;
	}
	else
	{
		m_FlashTick += GlobalLinkData::GetTick();
		if (m_FlashTick >= m_FlashTime)
		{
			m_FlashTick = 0.0f;
			if (m_Flash->IsActive()) m_Flash->SetActive(false);
			else m_Flash->SetActive(true);
		}
	}
}

void JGUIInputText::FocusExit(const JGUIFocusExitEvent& e)
{
	m_FlashTick = 0.0f;
	m_Flash->SetActive(false);
	Reset();
}

void JGUIInputText::Resize(const JGUIResizeEvent& e)
{
	Setting();

}

void JGUIInputText::SetTextSize(float size)
{
	m_TextSize = size;
	Setting();
	ReviseFlashPosByChar();
}



void JGUIInputText::Setting()
{
	// 플래쉬 조정
    float gap_y = m_TextSize * (m_GapRatio - 1.0f) * 0.5f;
	JVector2 flash_pos = m_Flash->GetTransform()->GetLocalPosition();
	flash_pos.y = gap_y;
	m_Flash->GetTransform()->SetLocalPosition(flash_pos);

	// 선택 박스 (파란색) 조정
	JVector2 selected_box_pos = m_SelectedBox->GetTransform()->GetLocalPosition();
	selected_box_pos.y = gap_y;
	m_SelectedBox->GetTransform()->SetLocalPosition(selected_box_pos);
	JVector2 selected_box_size = m_SelectedBox->GetTransform()->GetSize();
	selected_box_size.y = m_TextSize;
	m_SelectedBox->GetTransform()->SetSize(selected_box_size);


	// 텍스트 배경 박스 조정
	float box_size = m_TextSize * m_GapRatio;
	auto size = GetTransform()->GetSize();
	size.y = box_size;
	GetTransform()->SetSize(size);
	m_TextBackGround->GetTransform()->SetSize(size);
	m_TextBackGround->SetColor(JColor(1, 1, 1, 1));


	// 텍스트 크기 조정
	m_Text->SetTextRect(size.x, size.y);
	m_Flash->GetTransform()->SetSize(m_FlashThickness, m_TextSize);
	m_Text->SetFontSize(m_TextSize);
}
void JGUIInputText::InputText(const std::string& str)
{
	if (m_SelectedBox->IsActive())
	{
		m_Text->RemoveRange(m_SelectedStartTextIndex + 1, m_SelectedEndTextIndex + 1);
		m_CurrentFlashTextIndex = m_SelectedStartTextIndex;
		m_SelectedBox->SetActive(false);
	}


	m_Text->Insert(++m_CurrentFlashTextIndex, str);
	ReviseFlashPosByChar();

}
void JGUIInputText::RemoveText()
{
	if (m_SelectedBox->IsActive())
	{
		m_Text->RemoveRange(m_SelectedStartTextIndex + 1, m_SelectedEndTextIndex + 1);
		m_CurrentFlashTextIndex = m_SelectedStartTextIndex;
		m_SelectedBox->SetActive(false);
		ReviseFlashPosByChar();
	}
	else if (m_CurrentFlashTextIndex != -1)
	{
		m_Text->RemoveRange(m_CurrentFlashTextIndex, m_CurrentFlashTextIndex + 1);
		m_CurrentFlashTextIndex--;
		ReviseFlashPosByChar();
	}
}
void JGUIInputText::ReviseFlashPosByMousePos()
{
	auto mouse_pos = GetOwnerWindow()->GetMousePos();

	if (m_PrevMousePos == mouse_pos)
	{
		m_SelectedStartTextIndex = std::min<int>(m_SelectedStartTextIndex, m_CurrentFlashTextIndex);
		m_SelectedEndTextIndex = std::max<int>(m_SelectedEndTextIndex, m_CurrentFlashTextIndex);
		return;
	}
	m_PrevMousePos = mouse_pos;
	



	bool is_find = false;
	int is_find_text_index = -1;

	uint32_t text_len = m_Text->Length();
	float text_com_x = m_Text->GetTransform()->GetPosition().x;
	for (int i = 0; i < text_len; ++i)
	{
		auto text_start_pos = m_Text->GetTextPos(i);
		auto text_last_pos  = m_Text->GetTextLastPos(i);
		text_start_pos.x += text_com_x;
		text_last_pos.x += text_com_x;
		auto text_mid_pos_x = (text_start_pos.x + text_last_pos.x) * 0.5f;

		if (mouse_pos.x <= text_mid_pos_x && text_start_pos.x< mouse_pos.x)
		{
			is_find_text_index = i - 1;
			is_find = true;
			break;
		}
		if (mouse_pos.x <= text_last_pos.x && text_mid_pos_x < mouse_pos.x)
		{
			is_find_text_index = i;
			is_find = true;
			break;
		}
	}
	if (is_find) m_CurrentFlashTextIndex = is_find_text_index;
	else m_CurrentFlashTextIndex = text_len - 1;
	m_SelectedStartTextIndex = std::min<int>(m_SelectedStartTextIndex, m_CurrentFlashTextIndex);
	m_SelectedEndTextIndex = std::max<int>(m_SelectedEndTextIndex, m_CurrentFlashTextIndex);


	ReviseFlashPosByChar();
}

void JGUIInputText::ReviseFlashPosByChar()
{
	uint32_t len = m_Text->Length();
	if (len <= m_CurrentFlashTextIndex && m_CurrentFlashTextIndex != -1)
	{
		m_CurrentFlashTextIndex = len - 1;
	}

	auto text_last_pos = m_Text->GetTextLastPos(m_CurrentFlashTextIndex);
	auto pos = m_Flash->GetTransform()->GetLocalPosition();
	pos.x = text_last_pos.x;
	m_Flash->GetTransform()->SetLocalPosition(pos);
}

void JGUIInputText::FlashMove(const JGUIKeyDownEvent& e)
{


	if (e.Code == KeyCode::Right) m_CurrentFlashTextIndex++;
	if (e.Code == KeyCode::Left && m_CurrentFlashTextIndex != -1)  m_CurrentFlashTextIndex--;

	if(m_CurrentFlashTextIndex != -1)  m_CurrentFlashTextIndex = std::min<uint32_t>(m_Text->Length() - 1, m_CurrentFlashTextIndex);
	



	ReviseFlashPosByChar();
}

void JGUIInputText::Reset()
{
	m_PressedTick = 0.0f;
	m_FlashMove = false;
	m_CurrentChar = "";
	m_Selected = false;
}
