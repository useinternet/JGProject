#include"pch.h"
#include "JGUIInputText.h"
#include "JGUIPanel.h"
#include "JGUIShape.h"
#include "GUI/JGUIObject/JGUIWindow.h"


void JGUIInputText::Awake()
{
	JGUIElement::Awake();
	RegisterCollider(JGUI_Collider_Box);
	m_TextBackGround = CreateJGUIElement<JGUIPanel>("TextBackGround");
	m_Text = CreateJGUIElement<JGUITextMesh>("Text");
	m_SelectedBox = CreateJGUIElement<JGUIRectangle>("SelectedBox");
	m_SelectedBox->GetTransform()->SetSize(0, 0);
	m_Flash = CreateJGUIElement<JGUIRectangle>("Line");

}
void JGUIInputText::Start()
{
	JGUIElement::Start();
	m_SelectedBox->SetParent(m_TextBackGround);
	m_Text->SetParent(m_TextBackGround);
	m_Flash->SetParent(m_TextBackGround);
	



	Setting();
	Reset();
	m_Text->SetText("(none)");
	m_CurrentFlashTextIndex = m_Text->Length() - 1;
	ReviseFlashPosByChar();



	m_Text->SetColor(JColor(0, 0, 0, 1));
	m_Text->SetTextDrawing(JGUI_Text_Drawing_RightPushed);
	m_Flash->SetColor(JColor(0.0f, 0.0f, 0.0f, 1.0f));
	m_SelectedBox->SetColor(JColor(0.0f, 0.0f, 1.0f, 1.0f));
	m_Flash->SetActive(false);
	m_SelectedBox->SetActive(false);
	
}
void JGUIInputText::MouseBtDown(const JGUIKeyDownEvent& e)
{
	if (e.Code == KeyCode::LeftMouseButton && !m_Selected)
	{
		JGUIExtraEvent extra_event;
		extra_event.Bind(JGUI_EXTRAEVENT_REPEAT, this, [&](JGUIExtraEvent& e)
		{
			float tick = GlobalLinkData::GetTick();
			if (JGUI::GetKeyDown(GetOwnerWindow(), KeyCode::LeftMouseButton))
			{
				if (!m_Selected)
				{
					m_SelectedEndTextIndex = -2;
					m_SelectedStartTextIndex = INT_MAX;
				}
				m_Selected = true;
				auto mouse_pos = GetOwnerWindow()->GetMousePos();
				auto text_rect = m_Text->GetLocalTextRect();
				text_rect.left += m_Text->GetTransform()->GetPosition().x;
				text_rect.right += m_Text->GetTransform()->GetPosition().x;

				if (mouse_pos.x >= text_rect.left && mouse_pos.x <= text_rect.right)
				{
					m_MouseDownTick = 0.0f;
					ReviseFlashPosByMousePos();
				}
				else if(m_SelectedStartTextIndex <= m_SelectedEndTextIndex)
				{
					m_MouseDownTick += tick;
					// LeftPush
					if (mouse_pos.x <= text_rect.left && m_MouseDownTick >= 0.05f)
					{
						m_SelectedStartTextIndex--;
						m_CurrentFlashTextIndex = m_SelectedStartTextIndex;
	
						m_SelectedStartTextIndex = std::max<int>(m_SelectedStartTextIndex, -1);
						m_MouseDownTick = 0.0f;
					}
					// RightPush
					if (mouse_pos.x >= text_rect.right && m_MouseDownTick >= 0.05f)
					{
						m_SelectedEndTextIndex++;
						m_CurrentFlashTextIndex = m_SelectedEndTextIndex;
				
						m_SelectedEndTextIndex = std::min<int>(m_SelectedEndTextIndex, m_Text->Length() - 1);
						m_MouseDownTick = 0.0f;
					}
					
					ReviseFlashPosByChar();
				}


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

					size.x = end_pos.x - start_pos.x;
					pos.x = start_pos.x + JGUI::Gap();
					if (pos.x < JGUI::Gap())
					{
						float delta = JGUI::Gap() - pos.x;
						size.x -= delta;
						pos.x = JGUI::Gap();
					}


					
					auto rect = m_Text->GetTransform()->GetRect();
					rect.right = rect.left + m_Text->GetLocalTextRect().width();

					if (mouse_pos.x <= rect.left)
					{
						m_Text->PushLeft(m_SelectedStartTextIndex);
					}
					if (mouse_pos.x >= rect.right)
					{
						m_Text->PushRight(m_SelectedEndTextIndex + 1);
					}

					m_SelectedBox->GetTransform()->SetLocalPosition(pos);
					auto text_size = m_Text->GetLocalTextRect().width();
					if (size.x > text_size) size.x = text_size - pos.x;
					m_SelectedBox->GetTransform()->SetSize(size);
				}
			}


			if (m_Selected && JGUI::GetKeyUp(GetOwnerWindow(), KeyCode::LeftMouseButton))
			{
				m_Selected = false;
				e.flag = JGUI_EXTRAEVENT_EXIT;
			}
		});
		JGUI::RegisterExtraEvent(extra_event);
	}
}

void JGUIInputText::KeyDown(const JGUIKeyDownEvent& e)
{
	if (m_InputSwitch)
	{
		m_PressedTick += GlobalLinkData::GetTick();
	}	


	// 키 입력
	if (!m_FlashMove)
	{
		if (m_CurrentChar != "" &&  m_PressedTick >= 0.25f && m_InputSwitch)
		{
			InputText(m_CurrentChar);
			m_PressedTick = 0.0f;
		}
		if (m_CurrentChar == "\b" && e.Code == KeyCode::BackSpace && m_PressedTick >= 0.25f && m_InputSwitch)
		{
			RemoveText();
			m_PressedTick = 0.0f;
		}
		if (e.Code == KeyCode::Right || e.Code == KeyCode::Left)
		{
			m_FlashMoveTime = 0.5f;
			m_FlashMove = true;
			FlashMove(e);
		}
	}
	else
	{
		m_FlashMoveTick += GlobalLinkData::GetTick();
		if (m_FlashMoveTick >= m_FlashMoveTime)
		{
			m_FlashMoveTick = 0.0f;
			m_FlashMoveTime = 0.05f;
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
	m_InputSwitch = true;
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

float JGUIInputText::GetTextSize()
{
	return m_Text->GetFontSize();
}

const std::string& JGUIInputText::GetText()
{
	return m_Text->GetString();
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
	JVector2 selected_box_size = m_SelectedBox->GetTransform()->GetSize();
	selected_box_size.y = m_TextSize;
	m_SelectedBox->GetTransform()->SetSize(selected_box_size);
	m_SelectedBox->GetTransform()->SetLocalPosition(selected_box_pos);

	// 텍스트 배경 박스 조정
	float box_size = m_TextSize * m_GapRatio;
	auto size = GetTransform()->GetSize();
	size.y = box_size;
	GetTransform()->SetSize(size);
	m_TextBackGround->GetTransform()->SetSize(size);
	m_TextBackGround->SetColor(JColor(1, 1, 1, 1));


	// 텍스트 크기 조정
	m_Text->SetTextRect(size.x - (JGUI::Gap() * 2), size.y);
	m_Text->GetTransform()->SetLocalPosition(JGUI::Gap(), 0.0f);
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
		if (m_SelectedEndTextIndex == m_SelectedStartTextIndex)
		{
			if (m_SelectedStartTextIndex > m_CurrentFlashTextIndex) m_SelectedStartTextIndex = m_CurrentFlashTextIndex;
			else m_SelectedEndTextIndex = m_CurrentFlashTextIndex;
		}
		else if (m_SelectedStartTextIndex < m_CurrentFlashTextIndex &&
			m_SelectedEndTextIndex   > m_CurrentFlashTextIndex)
		{

			auto start_delta = m_CurrentFlashTextIndex - m_SelectedStartTextIndex;
			auto end_delta = m_SelectedEndTextIndex - m_CurrentFlashTextIndex;
			if (start_delta > end_delta) m_SelectedEndTextIndex = m_CurrentFlashTextIndex;
			else  m_SelectedStartTextIndex = m_CurrentFlashTextIndex;
		}
		else
		{
			m_SelectedStartTextIndex = std::min<int>(m_SelectedStartTextIndex, m_CurrentFlashTextIndex);
			m_SelectedEndTextIndex = std::max<int>(m_SelectedEndTextIndex, m_CurrentFlashTextIndex);
		}
		
		return;
	}
	m_PrevMousePos = mouse_pos;

	// 34  67

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
	if (is_find)
	{
		m_CurrentFlashTextIndex = is_find_text_index;
	}
	else
	{
		auto t_pos = m_Text->GetTextPos(text_len - 1).x;
		t_pos += m_Text->GetTransform()->GetPosition().x;
		if (mouse_pos.x >= t_pos)
		{
			m_CurrentFlashTextIndex = text_len - 1;
		}
		else
		{
			m_CurrentFlashTextIndex = -1;
		}

	}

	if (m_SelectedEndTextIndex == m_SelectedStartTextIndex)
	{

		if (m_SelectedStartTextIndex > m_CurrentFlashTextIndex) m_SelectedStartTextIndex = m_CurrentFlashTextIndex;
		else m_SelectedEndTextIndex = m_CurrentFlashTextIndex;
	}
	else if (m_SelectedStartTextIndex < m_CurrentFlashTextIndex &&
		m_SelectedEndTextIndex   > m_CurrentFlashTextIndex)
	{
	
		auto start_delta = m_CurrentFlashTextIndex - m_SelectedStartTextIndex;
		auto end_delta = m_SelectedEndTextIndex - m_CurrentFlashTextIndex;
		if (start_delta > end_delta) m_SelectedEndTextIndex = m_CurrentFlashTextIndex;
		else  m_SelectedStartTextIndex = m_CurrentFlashTextIndex;
	}
	else
	{
		m_SelectedStartTextIndex = std::min<int>(m_SelectedStartTextIndex, m_CurrentFlashTextIndex);
		m_SelectedEndTextIndex = std::max<int>(m_SelectedEndTextIndex, m_CurrentFlashTextIndex);
	}

	ReviseFlashPosByChar();
}

void JGUIInputText::ReviseFlashPosByChar()
{
	int len = m_Text->Length();
	m_CurrentFlashTextIndex = std::max<int>(-1, m_CurrentFlashTextIndex);
	if (len <= m_CurrentFlashTextIndex)
	{
		m_CurrentFlashTextIndex = len - 1;
	}

	auto text_last_pos = m_Text->GetTextLastPos(m_CurrentFlashTextIndex);
	auto pos = m_Flash->GetTransform()->GetLocalPosition();
	pos.x = text_last_pos.x;
	pos.x += m_Text->GetTransform()->GetLocalPosition().x;
	m_Flash->GetTransform()->SetLocalPosition(pos);
}

void JGUIInputText::FlashMove(const JGUIKeyDownEvent& e)
{


	if (e.Code == KeyCode::Right)
	{
		m_CurrentFlashTextIndex++;
		m_Text->PushRight(m_CurrentFlashTextIndex);
	}
	if (e.Code == KeyCode::Left && m_CurrentFlashTextIndex != -1)
	{
		
		m_CurrentFlashTextIndex--;
		m_Text->PushLeft(m_CurrentFlashTextIndex + 1);
	}

	m_CurrentFlashTextIndex = std::min<int>(m_Text->Length() - 1, m_CurrentFlashTextIndex);


	ReviseFlashPosByChar();
}

void JGUIInputText::Reset()
{
	m_PressedTick = 0.0f;
	m_FlashMoveTick = 0.0f;
	m_MouseDownTick = 0.0f;
	m_FlashMove = false;
	m_InputSwitch = false;
	m_CurrentChar = "";
	m_Selected = false;
	m_SelectedBox->SetActive(false);
	auto size= m_SelectedBox->GetTransform()->GetSize();
	size.x = 0.0f;
	m_SelectedBox->GetTransform()->SetSize(size);

	m_SelectedStartTextIndex = INT_MAX;
	 m_SelectedEndTextIndex = -2;
}
