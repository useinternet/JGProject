#include "pch.h"
#include "JGUIButton.h"
#include "JGUIShape.h"
#include "JGUIRectTransform.h"
#include "JGUICollider.h"
#include "GUI/JGUIObject/JGUIWindow.h"
using namespace std;

void JGUIButton::Awake()
{
	RegisterBoxCollider();
	GetTransform()->SetPosition(0.0f, 0.0f);
	m_Rectangle = CreateJGUIComponent<JGUIRectangle>("JGUIRectangle");
	m_Rectangle->GetTransform()->SetSize(GetTransform()->GetSize());

	m_BtColor[JGUI_BtState_Normal] = JColor(0.9f, 0.9f, 0.9f, 1.0f);
	m_BtColor[JGUI_BtState_Hightlight] = JColor(0.7f, 0.7f, 0.7f, 1.0f);
	m_BtColor[JGUI_BtState_Pressed] = JColor(0.5f, 0.5f, 0.5f, 1.0f);
	m_BtColor[JGUI_BtState_Selected] = JColor(0.8f, 0.8f, 0.9f, 1.0f);
	m_BtColor[JGUI_BtState_Disabled] = JColor(0.3f, 0.3f, 0.35f, 1.0f);

}
void JGUIButton::Start()
{
	if (m_SourceImage != "none")
	{
		m_Rectangle->SetImage(m_SourceImage);
	}
	m_Rectangle->SetColor(m_BtColor[m_BtState]);
}
void JGUIButton::Resize(const JGUIResizeEvent& e)
{
	m_Rectangle->GetTransform()->SetSize(e.width, e.height);
}
void JGUIButton::MouseBtUp(const JGUIKeyUpEvent& e)
{
	if (e.Code != KeyCode::LeftMouseButton) return;
	ENGINE_LOG_INFO(e.ToString());
	if (m_BtState == JGUI_BtState_Pressed)
	{
		m_Rectangle->SetColor(m_BtColor[JGUI_BtState_Selected]);
		m_BtState = JGUI_BtState_Selected;
		m_IsClick = true;
	}
}
void JGUIButton::MouseBtDown(const JGUIKeyDownEvent& e)
{
	if (e.Code != KeyCode::LeftMouseButton) return;
	m_Rectangle->SetColor(m_BtColor[JGUI_BtState_Pressed]);
	m_BtState = JGUI_BtState_Pressed;
}
void JGUIButton::MouseMove(const JGUIMouseMoveEvent& e)
{
	if (m_BtState == JGUI_BtState_Pressed) return;
	m_Rectangle->SetColor(m_BtColor[JGUI_BtState_Hightlight]);
	m_BtState = JGUI_BtState_Hightlight;
}

void JGUIButton::MouseLeave()
{
	if (m_BtState != JGUI_BtState_Hightlight) return;

	m_Rectangle->SetColor(m_BtColor[JGUI_BtState_Normal]);
	m_BtState = JGUI_BtState_Normal;

}

void JGUIButton::Tick(const JGUITickEvent& e)
{
	if (m_BtState == JGUI_BtState_Selected && JGUI::GetKeyDown(GetOwnerWindow(), KeyCode::LeftMouseButton))
	{
		HWND hWnd = GetOwnerWindow()->GetRootWindowHandle();
		if (!GetCollider()->CheckInPoint(JGUI::GetMousePos(hWnd)))
		{
			m_Rectangle->SetColor(m_BtColor[JGUI_BtState_Normal]);
			m_BtState = JGUI_BtState_Normal;
		}
	}
	if (m_BtState == JGUI_BtState_Pressed && JGUI::GetKeyUp(GetOwnerWindow(), KeyCode::LeftMouseButton))
	{
		HWND hWnd = GetOwnerWindow()->GetRootWindowHandle();
		if (!GetCollider()->CheckInPoint(JGUI::GetMousePos(hWnd)))
		{
			m_Rectangle->SetColor(m_BtColor[JGUI_BtState_Normal]);
			m_BtState = JGUI_BtState_Normal;
		}
	}
	if (m_IsClick)
	{
		OnClick();
		ENGINE_LOG_INFO("Bt Click");
		m_IsClick = false;
	}
}

void JGUIButton::SetBtImage(const std::string& path)
{
	m_SourceImage = path;
	m_Rectangle->SetImage(m_SourceImage);
}

void JGUIButton::SetBtColor(EJGUIBtState state, const JColor& color)
{
	if (state >= JGUI_BtState_Count) return;
	m_BtColor[state] = color;
}

void JGUIButton::BIndOnClick(const std::function<void()>& func)
{
	m_OnClick = func;
}

void JGUICloseButton::Start()
{
	JGUIButton::Start();
	SetBtImage("Close.png");
	
	SetBtColor(JGUI_BtState_Pressed, JColor(0.8f, 0.3f, 0.3f, 1.0f));
	SetBtColor(JGUI_BtState_Hightlight, JColor(1.0f, 0.0f, 0.0f, 1.0f));
}

void JGUICloseButton::OnClick()
{
	if (GetOwnerWindow())
	{
		auto window = GetOwnerWindow();
		JGUI::DestroyObject(window);
	}
	ENGINE_LOG_INFO("Close On Click");
}

void JGUIMaximizeButton::Start()
{
	JGUIButton::Start();
	SetBtImage("Maximize.png");
	SetBtColor(JGUI_BtState_Pressed, JColor(0.8f, 0.3f, 0.3f, 1.0f));
	SetBtColor(JGUI_BtState_Hightlight, JColor(1.0f, 0.0f, 0.0f, 1.0f));
}

void JGUIMaximizeButton::OnClick()
{
	ENGINE_LOG_INFO("Maximize On Click");
}

void JGUIMinimizeButton::Start()
{
	JGUIButton::Start();
	SetBtImage("Minimize.png");
	SetBtColor(JGUI_BtState_Pressed, JColor(0.8f, 0.3f, 0.3f, 1.0f));
	SetBtColor(JGUI_BtState_Hightlight, JColor(1.0f, 0.0f, 0.0f, 1.0f));
}

void JGUIMinimizeButton::OnClick()
{
	ENGINE_LOG_INFO("Minimize On Click");
}
