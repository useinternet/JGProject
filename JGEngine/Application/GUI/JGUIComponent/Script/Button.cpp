#include "pch.h"
#include "Button.h"

#include "GUI/JGUIObject/JGUIElement.h"
#include "GUI/JGUIObject/JGUIWindow.h"

#include "GUI/JGUIComponent/Element/JGUIRenderer.h"
#include "GUI/JGUIComponent/JGUITransform.h"


using namespace std;
JGUI_REGISTER_COMPONENT(Button)

void Button::Awake()
{
	if (GetOwner()->GetCollider() == nullptr) GetOwner()->RegisterCollider(JGUI_Collider_Box);
	m_BtColors[Normal]     = JColor(0.9f, 0.9f, 0.9f, 1.0f);
	m_BtColors[Hightlight] = JColor(0.7f, 0.7f, 0.7f, 1.0f);
	m_BtColors[Pressed]    = JColor(0.5f, 0.5f, 0.5f, 1.0f);
	m_BtColors[Disabled]   = JColor(0.3f, 0.3f, 0.35f, 1.0f);
	for (int i = 0; i < BtStateCount; ++i)
	{
		m_ImageSources[i] = "none";
	}
}

void Button::Start()
{
	
	m_Image = GetOwner()->FindComponent<JGUIImageRenderer>();

	// MouseDown
	GetOwner()->BindMouseBtDownFunc([&](const JGUIKeyDownEvent& e)
	{
		if (e.Code != KeyCode::LeftMouseButton) return;
		if (m_BtState == Disabled) return;
		SetBtState(Pressed);
	});
	// MouseMove
	GetOwner()->BindMouseMoveFunc([&](const JGUIMouseMoveEvent& e)
	{
		if (m_BtState == Pressed || m_BtState == Disabled) return;
		SetBtState(Hightlight);

	});

	GetOwner()->BindMouseLeaveFunc([&]()
	{
		if (m_BtState == Disabled || m_BtState != Hightlight) return;
		SetBtState(Normal);
	});

	SetBtState(Normal);
}

void Button::Destroy()
{

}

void Button::Tick(float tick)
{
	if(m_Image == nullptr) 	m_Image = GetOwner()->FindComponent<JGUIImageRenderer>();

	if (m_BtState == Pressed && JGUI::GetKeyUp(GetOwner()->GetOwnerWindow(), KeyCode::LeftMouseButton))
	{
		auto p = GetOwner()->GetOwnerWindow()->GetMousePos();
		if (GetOwner()->GetCollider()->CheckInPoint(p))
		{
			SetBtState(Hightlight);
			m_IsClick = true;
		}
		else
		{
			SetBtState(Normal);
		}
	
	}
	if (m_BtState == Hightlight && !GetOwner()->GetCollider()->CheckInPoint(GetOwner()->GetOwnerWindow()->GetMousePos()))
	{
		SetBtState(Normal);
	}

	if (m_IsClick)
	{
		if (m_OnClick) m_OnClick();
		m_IsClick = false;
	}
}

void Button::BindOnClick(const std::function<void()>& func)
{
	m_OnClick = func;
}

void Button::SetImage(int btState, const std::string& image)
{
	m_ImageSources[btState] = image;
	if (m_BtState == btState && m_Image)
	{
		m_Image->SetImage(m_ImageSources[btState]);
	}
}

void Button::SetImage(const std::string& image)
{
	for (int i = 0; i < BtStateCount; ++i)
	{
		SetImage(i, image);
	}
}

void Button::SetColor(int btState, const JColor& color)
{
	m_BtColors[btState] = color;
	if (m_BtState == btState && m_Image)
	{
		m_Image->SetColor(m_BtColors[btState]);
	}
	
}

void Button::Reset()
{
	SetBtState(Normal);
	m_IsClick = false;
}

void Button::SetBtState(int state)
{
	if (m_Image == nullptr) return;
	m_BtState = state;
	m_Image->SetColor(m_BtColors[m_BtState]);
	m_Image->SetImage(m_ImageSources[m_BtState]);
}
