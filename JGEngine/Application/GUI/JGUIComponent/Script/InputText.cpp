#include "pch.h"
#include "InputText.h"
#include "GUI/JGUIObject/JGUIElement.h"
#include "GUI/JGUIObject/JGUIWindow.h"

#include "GUI/JGUIComponent/Element/JGUIRenderer.h"

#include "GUI/JGUIComponent/Script/Text.h"
#include "GUI/JGUIComponent/Script/GUIMask.h"
#include "GUI/JGUIFont/JGUIFontManager.h"
using namespace std;

void InputText::Awake()
{
	auto stickMask          = GetOwner()->CreateJGUIComponent<GUIMask>();
	m_InputBackGround       = GetOwner()->CreateJGUIComponent<JGUIImageRenderer>();
	m_InputBackGroundBorder = GetOwner()->CreateJGUIElement("InputBackGroundBorder")->CreateJGUIComponent<JGUIShapeRenderer>();
	m_TextArea              = GetOwner()->CreateJGUIElement("TextArea");
	
	m_Text                  = m_TextArea->CreateJGUIElement("Text")->CreateJGUIComponent<Text>();
	m_IStick                = m_Text->GetOwner()->CreateJGUIElement("InputStick")->CreateJGUIComponent<JGUIImageRenderer>();
	m_SelectBlock           = m_Text->GetOwner()->CreateJGUIElement("SelectBlock")->CreateJGUIComponent<JGUIShapeRenderer>();
	m_TextArea->CreateJGUIComponent<GUIMask>()->Bind(m_Text);
	m_TextArea->CreateJGUIComponent<GUIMask>()->Bind(m_SelectBlock);
	stickMask->Bind(m_IStick);
	

	m_InputBackGroundBorder->CreateEmptyRectangle(0.5f);
	GetTransform()->AttachTransform(m_InputBackGroundBorder->GetTransform());


	m_IStick->SetColor(JColor::Black());
	m_IStick->GetOwner()->SetActive(false);

	JColor color = JColor::Blue(); color.a = 0.5f;
	m_SelectBlock->SetColor(color);


	// Resize Bind
	GetOwner()->BindResizeFunc([&](const JGUIResizeEvent& e)
	{
		Setting();

	});

	// TextArea 상호작용 Bind
	m_TextArea->RegisterCollider(JGUI_Collider_Box);
	{
		// OnFocus
		m_TextArea->BindOnFocusFunc([&]()
		{
			// Stick 깜박임
			m_IStickFlashTick += GetTick();
			if (m_IStickFlashTick >= m_IStickFlashTime)
			{
				m_IStickFlashTick = 0.0f;
				if (m_IStick->GetOwner()->IsActive()) m_IStick->GetOwner()->SetActive(false);
				else m_IStick->GetOwner()->SetActive(true);
			}
		});


		// FocusEnter
		m_TextArea->BindFocusEnterFunc([&](const JGUIFocusEnterEvent& e)
		{
			// Stick 활성화
			m_IStick->GetOwner()->SetActive(true);

		});



		// FocusExit
		m_TextArea->BindFocusExitFunc([&](const JGUIFocusExitEvent& e)
		{
			// Stick 비활성화 및 선택 박스 리셋
			m_IStickFlashTick = 0.0f;
			m_IStick->GetOwner()->SetActive(false);

			ResetSelectBox();
		});


		// Char 
		m_TextArea->BindCharFunc([&](const JGUICharEvent& e)
		{

			// Enter
			if (e.str == "\r")
			{
				return;
			}
			// BackSpace
			else if (e.str == "\b")
			{
				// 텍스트가 비어져있거나, 현재 Stick의 인덱스가 0인경우 함수 탈출
				if (m_TextStr.empty()) return;
				if (m_CurrentStickIndex == 0) return;



				// 선택 박스가 지정되어있다면 그 부분 삭제 후 문자 삽입
				if (m_StartSelectIndex != -1 && m_EndSelectIndex != -1 &&
					m_StartSelectIndex != m_EndSelectIndex)
				{
					int left_index = std::min<int>(m_StartSelectIndex, m_EndSelectIndex);
					int right_index = std::max<int>(m_StartSelectIndex, m_EndSelectIndex);
					if (m_StartSelectIndex > m_EndSelectIndex) m_CurrentStickIndex -= (right_index - left_index);

					m_TextStr.erase(m_TextStr.begin() + left_index, m_TextStr.begin() + right_index);
					ResetSelectBox();
				}
				else
				{
					m_CurrentStickIndex--;
					m_TextStr.erase(m_TextStr.begin() + m_CurrentStickIndex);
				}


			}
			else
			{
				if (m_TextStr.empty()) m_CurrentStickIndex = 0;


				if (m_StartSelectIndex != -1 && m_EndSelectIndex != -1 &&
					m_StartSelectIndex != m_EndSelectIndex)
				{
					int left_index = std::min<int>(m_StartSelectIndex, m_EndSelectIndex);
					int right_index = std::max<int>(m_StartSelectIndex, m_EndSelectIndex);
					if (m_StartSelectIndex > m_EndSelectIndex) m_CurrentStickIndex -= (right_index - left_index);
					m_TextStr.erase(m_TextStr.begin() + left_index, m_TextStr.begin() + right_index);
					ResetSelectBox();
				}
				m_TextStr.insert(m_CurrentStickIndex, e.str);
				m_CurrentStickIndex++;
			}




			// 문자 조정 및 스틱 위치 변경
			m_Text->SetText(m_TextStr);
			JGUIRect textAreaRect = m_TextArea->GetTransform()->GetRect();
			float width           = m_Text->GetTextWidth();
			float stick_local_pos = GetTextLocalPos(m_CurrentStickIndex);

			m_IStick->GetTransform()->SetLocalPosition(stick_local_pos, 0.0f);
			float stick_pos = m_IStick->GetTransform()->GetPosition().x;

			// 넘어가면
			if (stick_pos > textAreaRect.right)
			{
				float delta      = textAreaRect.right - stick_pos;
				float text_width = m_Text->GetTextWidth();
				m_Text->GetTransform()->OffsetLocalPosition(delta, 0.0f);
			


				JVector2 text_localPos = m_Text->GetTransform()->GetLocalPosition();
				text_localPos.x = std::max<float>(textAreaRect.width() - text_width, text_localPos.x);
				m_Text->GetTransform()->SetLocalPosition(text_localPos);
			}

		});


		m_TextArea->BindMouseBtDownFunc([&](const JGUIKeyDownEvent& e)
		{
			if (!m_IsSelect && e.Code == KeyCode::LeftMouseButton)
			{
				ResetSelectBox();
				m_IsSelect = true;
				float text_offset_x = m_Text->GetTransform()->GetLocalPosition().x;

				auto int_mouse_pos = GetOwnerWindow()->GetMousePos();
				int_mouse_pos.x -= text_offset_x;
				m_StartSelectIndex = GetTextIndex({ (float)int_mouse_pos.x, (float)int_mouse_pos.y });



				JGUIExtraEvent e;
				e.Bind(JGUI_EXTRAEVENT_REPEAT, GetOwnerWindow(), [&](JGUIExtraEvent& e)
				{
					auto     int_mouse_pos = GetOwnerWindow()->GetMousePos();
					JGUIRect textArea_rect  = m_TextArea->GetTransform()->GetRect();
					float    text_offset_x = m_Text->GetTransform()->GetLocalPosition().x;
					if (int_mouse_pos.x < textArea_rect.left)
					{
						float delta = textArea_rect.left - int_mouse_pos.x;
						
						float local_x = std::min<float>(0.0f, text_offset_x + (delta * delta * GetTick()));
						m_Text->GetTransform()->SetLocalPosition(local_x, 0.0f);
					}
					else if (int_mouse_pos.x > textArea_rect.right)
					{
						float delta = int_mouse_pos.x - textArea_rect.right;
						float text_width = m_Text->GetTextWidth();
						float local_x = std::max<float>(textArea_rect.width() - text_width, text_offset_x - (delta * delta * GetTick()));
						m_Text->GetTransform()->SetLocalPosition(local_x, 0.0f);
					}
	


					int_mouse_pos.x -= text_offset_x;
					if (m_IsSelect)
					{

						m_EndSelectIndex = GetTextIndex({ (float)int_mouse_pos.x, (float)int_mouse_pos.y });
						CalcSelectBox();

					}


					if (m_IsSelect && JGUI::GetKeyUp(GetOwnerWindow(), KeyCode::LeftMouseButton))
					{
						m_IsSelect = false;
						m_EndSelectIndex = GetTextIndex({ (float)int_mouse_pos.x, (float)int_mouse_pos.y });
						CalcSelectBox();
						e.flag = JGUI_EXTRAEVENT_EXIT;
					}

				});
				JGUI::RegisterExtraEvent(e);

				float text_pos = GetTextLocalPos(m_StartSelectIndex);
				m_IStick->GetTransform()->SetLocalPosition(text_pos, 0.0f);
				m_CurrentStickIndex = m_StartSelectIndex;
			}
		});

		m_TextArea->BindKeyDownFunc([&](const JGUIKeyDownEvent& e)
		{
			if (e.Code != KeyCode::Right && e.Code != KeyCode::Left) return;
			if (m_FirstKeyInputTick < m_FirstInputDelay)
			{
				if (m_FirstKeyInputTick == 0.0f)
				{
					if (e.Code == KeyCode::Right)
					{
						++m_CurrentStickIndex;
						m_CurrentStickIndex = std::min<int>(m_CurrentStickIndex, (int)m_TextStr.length());
						
					}
					if (e.Code == KeyCode::Left)
					{
						--m_CurrentStickIndex;
						m_CurrentStickIndex = std::max<int>(0, m_CurrentStickIndex);
					}
				
					// stick position 변경
					float text_pos = GetTextLocalPos(m_CurrentStickIndex);
					m_IStick->GetTransform()->SetLocalPosition(text_pos, 0.0f);
				}

				m_FirstKeyInputTick += GetTick();
				m_FirstKeyInputTick = std::min<float>(m_FirstKeyInputTick, m_FirstInputDelay);
			}
			else
			{
				m_KeyInputTick += GetTick();
				m_KeyInputTick = std::min<float>(m_KeyInputTick, m_TickInputDelay);
				if (m_KeyInputTick >= m_TickInputDelay)
				{
					m_KeyInputTick = 0.0f;
					if (e.Code == KeyCode::Right)
					{
						++m_CurrentStickIndex;
						m_CurrentStickIndex = std::min<int>(m_CurrentStickIndex, (int)m_TextStr.length());

					}
					if (e.Code == KeyCode::Left)
					{
						--m_CurrentStickIndex;
						m_CurrentStickIndex = std::max<int>(0, m_CurrentStickIndex);
					}
					// stick position 변경
					float text_pos = GetTextLocalPos(m_CurrentStickIndex);
				
					m_IStick->GetTransform()->SetLocalPosition(text_pos, 0.0f);
				}
			}
	
		});

		m_TextArea->BindKeyUpFunc([&](const JGUIKeyUpEvent& e)
		{
			m_FirstKeyInputTick = 0.0f;
			m_KeyInputTick = 0.0f;




		});

	}

	


	Setting();
	m_Text->GetTransform()->SetSize(m_TextWidth, m_FontSize);
	m_Text->SetColor(JColor::Black());
}

void InputText::Start()
{
}

void InputText::Destroy()
{
}

void InputText::Tick(float tick)
{
	
}

void InputText::SetBackGroundImage(const std::string& sourceImagePath)
{
	m_InputBackGround->SetImage(sourceImagePath);
}

void InputText::SetFont(const std::string& fontname)
{
	ENGINE_LOG_FATAL("InputText::SetFont() : No imp");
}

void InputText::SetTextColor(const JColor& color)
{
	m_Text->SetColor(color);
}

void InputText::SetSelectBlockColor(const JColor& color)
{
	m_SelectBlock->SetColor(color);
}

const std::string& InputText::GetString() const
{
	return m_TextStr;
}

const std::string& InputText::GetBackGroundImage() const
{
	return m_InputBackGround->GetImage();
}

const std::string& InputText::GetFontName() const
{
	return m_Text->GetFontName();
	// TODO: 여기에 return 문을 삽입합니다.
}

const JColor& InputText::GetTextColor() const
{
	return m_Text->GetColor();
}

const JColor& InputText::GetSelectBlockColor() const
{
	return m_SelectBlock->GetColor();
}

void InputText::Setting()
{
	auto size = GetTransform()->GetSize();

	m_FontSize = size.y - (m_InputGap * 2);

	m_TextArea->GetTransform()->SetSize(size.x - (2 * m_InputGap), m_FontSize);
	m_TextArea->GetTransform()->SetLocalPosition(m_InputGap, m_InputGap);


	m_IStick->GetTransform()->SetSize(1.0f, m_FontSize);
	m_SelectBlock->GetTransform()->SetSize(0.0f, m_FontSize);
	m_Text->SetFontSize(m_FontSize);
}

int InputText::GetTextIndex(const JVector2& pos)
{
	
	if (m_TextStr.empty()) return -1;
	float text_start_x = GetTransform()->GetPosition().x;
	float text_end_x = text_start_x + m_Text->GetTextWidth();

	size_t text_len = m_TextStr.length();

	


	//
	float last_char_width = Text::GetTextWidth(m_Text->GetFontName(), m_FontSize, m_TextStr.substr(text_len - 1, 1));
	if (text_end_x - (last_char_width * 0.5f) < pos.x)
	{
		return text_len;
	}
	float first_char_width = Text::GetTextWidth(m_Text->GetFontName(), m_FontSize, m_TextStr.substr(0, 1));
	if (text_start_x + (first_char_width * 0.5f) > pos.x)
	{
		return 0;
	}

	// 123

	float offset_width = text_start_x + first_char_width;
	for (size_t i = 1; i < text_len; ++i)
	{ 
		float curr_char_width = Text::GetTextWidth(m_Text->GetFontName(), m_FontSize, m_TextStr.substr(i, 1));
		float prev_char_width = Text::GetTextWidth(m_Text->GetFontName(), m_FontSize, m_TextStr.substr(i - 1, 1));

		float left_x  = offset_width - (prev_char_width * 0.5f);
		float right_x = offset_width + (curr_char_width * 0.5f);
		if (left_x < pos.x && right_x >= pos.x)
		{
			return i;
		}


		offset_width += curr_char_width;
	}
	return 0;
}

float InputText::GetTextLocalPos(const uint32_t index)
{

	return Text::GetTextWidth(
		m_Text->GetFontName(), m_FontSize, m_TextStr.substr(0, index));
}

void InputText::CalcSelectBox()
{
	if (m_StartSelectIndex == -1 || m_EndSelectIndex == -1) return;
	if (m_StartSelectIndex == m_EndSelectIndex) return;



	int left_index  = std::min<int>(m_StartSelectIndex, m_EndSelectIndex);
	int right_index = std::max<int>(m_StartSelectIndex, m_EndSelectIndex);

	float block_width = Text::GetTextWidth(
		m_Text->GetFontName(), m_FontSize, m_TextStr.substr(left_index, right_index - left_index));

	float block_pos_x = Text::GetTextWidth(
		m_Text->GetFontName(), m_FontSize, m_TextStr.substr(0, left_index));


	JVector2 block_size = m_SelectBlock->GetTransform()->GetSize();

	m_SelectBlock->GetTransform()->SetSize(block_width, block_size.y);
	m_SelectBlock->GetTransform()->SetLocalPosition(block_pos_x, 0.0f);
}

void InputText::ResetSelectBox()
{
	m_StartSelectIndex = -1;
	m_EndSelectIndex   = -1;
	m_SelectBlock->GetTransform()->SetLocalPosition(0, 0);
	m_SelectBlock->GetTransform()->SetSize(0, m_FontSize);
}
