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

}
void JGUIButton::Resize(const JGUIResizeEvent& e)
{
	m_Rectangle->GetTransform()->SetSize(e.width, e.height);
}
void JGUIButton::MouseBtUp(const JGUIKeyUpEvent& e)
{
	if (e.Code != KeyCode::LeftMouseButton) return;

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
	if (m_BtState == JGUI_BtState_Pressed || m_BtState == JGUI_BtState_Selected) return;
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
