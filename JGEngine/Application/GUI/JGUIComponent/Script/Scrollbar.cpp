#include "pch.h"
#include "Scrollbar.h"


#include "GUI/JGUIObject/JGUIElement.h"
#include "GUI/JGUIObject/JGUIWindow.h"

#include "GUI/JGUIComponent/Element/JGUIRenderer.h"
JGUI_REGISTER_COMPONENT(Scrollbar)
void Scrollbar::Awake()
{



}

void Scrollbar::Start()
{
	Setting();

	GetOwner()->BindResizeFunc([&](const JGUIResizeEvent  &e)
	{
		Setting();
	});
}

void Scrollbar::Tick(float tick)
{

}

void Scrollbar::Destroy()
{
}

void Scrollbar::AdjustBar(float ratio)
{
	m_Ratio = ratio;
	m_Ratio = std::max<float>(0.0f, m_Ratio);
	m_Ratio = std::min<float>(m_TotalRatio, m_Ratio);
	if (m_ScrollBt)
	{
		JVector2 bar_pos;
		bar_pos = m_ScrollBt->GetTransform()->GetLocalPosition();
		JVector2 this_size = GetTransform()->GetSize();
		float bar_size = 0.0f;
		float max_pos = 0.0f;
		switch (m_BarType)
		{
		case Horizontal:
			bar_size = this_size.x * m_OccupyRatio;
			max_pos = this_size.x * (m_TotalRatio - m_OccupyRatio);
			bar_pos.x = max_pos * m_Ratio;
			m_ScrollBt->GetTransform()->SetSize(bar_size, this_size.y);
			m_ScrollBt->GetTransform()->SetLocalPosition(bar_pos);
			break;
		case Vertical:
			bar_size = this_size.y * m_OccupyRatio;
			max_pos = this_size.y * (m_TotalRatio - m_OccupyRatio);
			bar_pos.y = max_pos * m_Ratio;
			m_ScrollBt->GetTransform()->SetSize(this_size.x, bar_size);
			m_ScrollBt->GetTransform()->SetLocalPosition(bar_pos);
			break;
		}
	}
}

void Scrollbar::SetBarType(int n)
{
	m_BarType = n;
	Setting();
}

void Scrollbar::Setting()
{
	auto size = GetTransform()->GetSize();

	if (m_ScrollBackGround == nullptr)
	{
		m_ScrollBackGround = GetOwner()->CreateJGUIElement("ScrollBackground")->CreateJGUIComponent<JGUIImageRenderer>();
		m_ScrollBackGround->SetColor(JColor::White());
		GetTransform()->AttachTransform(m_ScrollBackGround->GetTransform());
	}

	if (m_ScrollBt == nullptr)
	{
		m_ScrollBt = GetOwner()->CreateJGUIElement("ScrollBar")->CreateJGUIComponent<ScrollButton>();
		m_ScrollBt->GetOwner()->CreateJGUIComponent<JGUIImageRenderer>();
		m_ScrollBt->m_OwnerScrollbar = this;
	}
	AdjustBar(m_Ratio);
}



void Scrollbar::ScrollButton::Awake()
{
	Button::Awake();
	SetColor(Normal, JColor(0.4f, 0.4f, 0.45f, 1.0f));
	SetColor(Hightlight, JColor(0.5f, 0.5f, 0.55f, 1.0f));
	SetColor(Pressed, JColor(0.6f, 0.6f, 0.65f, 1.0f));
}

void Scrollbar::ScrollButton::Start()
{
	Button::Start();

	GetOwner()->BindMouseBtDownFunc([&](const JGUIKeyDownEvent& e)
	{
		if (e.Code == KeyCode::LeftMouseButton && !m_IsGrap)
		{
			m_IsGrap = true;
			JGUIExtraEvent e;
			m_PrevMousePos = GetOwnerWindow()->GetMousePos();
			m_PrevRatio = m_OwnerScrollbar->GetCurrentRatio();
			e.Bind(JGUI_EXTRAEVENT_REPEAT, GetOwner(), [&](JGUIExtraEvent& e)
			{
				if (m_IsGrap)
				{
					auto mouse_pos = GetOwnerWindow()->GetMousePos();
					JVector2Int delta;
					delta.x = mouse_pos.x - m_PrevMousePos.x;
					delta.y = mouse_pos.y - m_PrevMousePos.y;


					JVector2 owner_size = m_OwnerScrollbar->GetTransform()->GetSize();
					float occupy_ratio = (1.0f - m_OwnerScrollbar->GetOccupyRatio());
					// + 면 오른쪽, 아래쪽
					// - 면 왼쪽, 위쪽
					float ratio = 0.0f;
					switch (m_OwnerScrollbar->GetBarType())
					{
					case Horizontal:
						ratio = delta.x / (owner_size.x * occupy_ratio);
						break;
					case Vertical:
						ratio = delta.y / (owner_size.y * occupy_ratio);
						break;
					}
					m_OwnerScrollbar->AdjustBar(m_PrevRatio + ratio);
				}

				if (JGUI::GetKeyUp(GetOwnerWindow(), KeyCode::LeftMouseButton))
				{
					m_IsGrap = false;
					e.flag = JGUI_EXTRAEVENT_EXIT;
				}


			});
			JGUI::RegisterExtraEvent(e);
		}

	});



}
