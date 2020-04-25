#include "pch.h"
#include "JGUIScrollbar.h"
#include "JGUIShape.h"
#include "GUI/JGUIForm/JGUIForm.h"
using namespace std;

void JGUIScrollbar::Awake()
{
	JGUIElement::Awake();
	ElementSetting();

}

void JGUIScrollbar::Resize(const JGUIResizeEvent& e)
{
	ElementSetting();

}

void JGUIScrollbar::ElementSetting()
{
	
	JVector2 window_size = GetOwnerWindow()->GetTransform()->GetSize();
	JVector2 this_size = GetTransform()->GetSize();



	if (m_ScrollBackGround == nullptr)
	{
		m_ScrollBackGround = CreateJGUIElement<JGUIImage>("ScrollbarBackGround");
		m_ScrollBackGround->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
		GetTransform()->AttachTransform(m_ScrollBackGround->GetTransform());
	}
	
	if (m_Bar == nullptr)
	{
		m_Bar = CreateJGUIElement<JGUIButtonBar>("Bar");
		m_Bar->SetParent(m_ScrollBackGround);
		m_Bar->m_Owner = this;

	}
	AdjustBar(m_Ratio);

}

void JGUIScrollbar::SetBarType(int n)
{
	m_BarType = n;
	ElementSetting();
}
void JGUIScrollbar::Set(float occupyRatio)
{
	m_OccupyRatio = occupyRatio;
	ElementSetting();
}
void JGUIScrollbar::AdjustBar(float ratio)
{
	m_Ratio = ratio;
	m_Ratio = std::max<float>(0.0f, m_Ratio);
	m_Ratio = std::min<float>(m_TotalRatio, m_Ratio);
	if (m_Bar)
	{
		JVector2 bar_pos;
		bar_pos = m_Bar->GetTransform()->GetLocalPosition();
		JVector2 this_size = GetTransform()->GetSize();
		float bar_size = 0.0f;
		float max_pos = 0.0f;
		switch (m_BarType)
		{
		case Horizontal:
			bar_size = this_size.x * m_OccupyRatio;
			max_pos = this_size.x * (m_TotalRatio - m_OccupyRatio);
			bar_pos.x = max_pos * m_Ratio;
			m_Bar->GetTransform()->SetSize(bar_size, this_size.y);
			m_Bar->GetTransform()->SetLocalPosition(bar_pos);
			break;
		case Vertical:
			bar_size = this_size.y * m_OccupyRatio;
			max_pos = this_size.y * (m_TotalRatio - m_OccupyRatio);
			bar_pos.y = max_pos * m_Ratio;
			m_Bar->GetTransform()->SetSize(this_size.x, bar_size);
			m_Bar->GetTransform()->SetLocalPosition(bar_pos);
			break;
		}
	}

}

void JGUIScrollbar::JGUIButtonBar::Awake()
{
	JGUIButton::Awake();
	SetBtColor(JGUI_BtState_Normal, JColor(0.4f, 0.4f, 0.45f, 1.0f));
	SetBtColor(JGUI_BtState_Hightlight, JColor(0.5f, 0.5f, 0.55f, 1.0f));
	SetBtColor(JGUI_BtState_Pressed, JColor(0.6f, 0.6f, 0.65f, 1.0f));
}

void JGUIScrollbar::JGUIButtonBar::MouseBtDown(const JGUIKeyDownEvent& e)
{
	JGUIButton::MouseBtDown(e);
	if (e.Code == KeyCode::LeftMouseButton && !m_IsGrap)
	{
		m_IsGrap = true;
		JGUIExtraEvent e;
		m_PrevMousePos = GetOwnerWindow()->GetMousePos();
		m_PrevRatio = m_Owner->GetCurrentRatio();
		e.Bind(JGUI_EXTRAEVENT_REPEAT, this, [&](JGUIExtraEvent& e)
		{
			if (m_IsGrap)
			{
				auto mouse_pos = GetOwnerWindow()->GetMousePos();
				JVector2Int delta;
				delta.x = mouse_pos.x - m_PrevMousePos.x;
				delta.y = mouse_pos.y -  m_PrevMousePos.y;


				JVector2 owner_size = m_Owner->GetTransform()->GetSize();
				float occupy_ratio = (1.0f - m_Owner->GetOccupyRatio());
				// + 면 오른쪽, 아래쪽
				// - 면 왼쪽, 위쪽
				float ratio = 0.0f;
				switch (m_Owner->GetBarType())
				{
				case Horizontal:
					ratio = delta.x / (owner_size.x * occupy_ratio);
					break;
				case Vertical:
					ratio = delta.y / (owner_size.y * occupy_ratio);
					break;
				}
				ENGINE_LOG_INFO("{0}", m_PrevRatio);
				m_Owner->AdjustBar(m_PrevRatio + ratio);
			}
		
			if (JGUI::GetKeyUp(GetOwnerWindow(), KeyCode::LeftMouseButton))
			{
				m_IsGrap = false;
				e.flag = JGUI_EXTRAEVENT_EXIT;
			}


		});
		JGUI::RegisterExtraEvent(e);

		
	}




}
