#include "pch.h"
#include "TitleBar.h"

#include "GUI/JGUIObject/JGUIElement.h"
#include "GUI/JGUIObject/JGUIWindow.h"

#include "GUI/JGUIComponent/Element/JGUICanvas.h"
#include "GUI/JGUIComponent/Element/JGUIRenderer.h"

#include "GUI/JGUIComponent/Script/Button.h"
#include "GUI/JGUIComponent/Script/DockSystem.h"

using namespace std;




void TitleBar::Awake()
{
	if (GetOwner()->GetCollider() == nullptr) GetOwner()->RegisterCollider(JGUI_Collider_Box);

	// 캔버스 생성
	auto canvas = GetOwner()->CreateJGUIElement("Canvas");
	GetOwner()->GetTransform()->AttachTransform(canvas->GetTransform());


	m_Image       = canvas->CreateJGUIComponent<JGUIImageRenderer>();
	m_Image->SetColor(0.3f, 0.3f, 0.3f, 1.0f);
	m_TitleCanvas = canvas->CreateJGUIComponent<JGUICanvas>();
	m_TitleCanvas->SetCanvasFlags(JGUI_CanvasFlag_Overlay);
}

void TitleBar::Start()
{


	GetOwner()->BindMouseBtDownFunc([&](const JGUIKeyDownEvent& e)
	{
		MouseBtDown(e);
	});
	GetOwner()->BindResizeFunc([&](const JGUIResizeEvent& e)
	{
		SettingByWindowFlags();
	});
	SettingByWindowFlags();
}

void TitleBar::Destroy()
{
}

void TitleBar::Tick(float tick)
{
}

void TitleBar::UnConnect()
{
	m_IsUnConnect = true;
}

void TitleBar::Connect(const JVector2Int& delta)
{
	JGUIKeyDownEvent e;
	e.Code = KeyCode::LeftMouseButton;
	MouseBtDown(e);
	m_Delta = delta;
}

void TitleBar::SettingByWindowFlags()
{
	auto canvas = m_TitleCanvas->GetOwner();
	auto win_flags = GetOwnerWindow()->GetWindowFlags();

	//
	if (win_flags & JGUI_WindowFlag_EnableClose)
	{
		if (m_CloseBt == nullptr)
		{
			auto close_bt_object = canvas->CreateJGUIElement("CloseBt");
			close_bt_object->CreateJGUIComponent<JGUIImageRenderer>();
			m_CloseBt = close_bt_object->CreateJGUIComponent<Button>();
			m_CloseBt->SetImage("Close.png");
			m_CloseBt->SetColor(Button::Pressed, JColor(0.8f, 0.3f, 0.3f, 1.0f));
			m_CloseBt->SetColor(Button::Hightlight, JColor(1.0f, 0.0f, 0.0f, 1.0f));
			m_CloseBt->BindOnClick([&]()
			{
				auto window = GetOwnerWindow();
				JGUI::DestroyObject(window);
			});
		}
	}
	else
	{
		if (m_CloseBt)
		{
			canvas->DestroyJGUIElement(m_CloseBt->GetOwner());
			m_CloseBt = nullptr;
		}
	}
	// 최대화 버튼 활성화
	if (win_flags & JGUI_WindowFlag_EnableMaximize)
	{
		if (m_MaxBt == nullptr)
		{
			auto max_bt_object = canvas->CreateJGUIElement("MaxBt");
			max_bt_object->CreateJGUIComponent<JGUIImageRenderer>();
			m_MaxBt = max_bt_object->CreateJGUIComponent<Button>();
			m_MaxBt->SetImage("Maximize.png");
			m_MaxBt->SetColor(Button::Pressed, JColor(0.8f, 0.3f, 0.3f, 1.0f));
			m_MaxBt->SetColor(Button::Hightlight, JColor(1.0f, 0.0f, 0.0f, 1.0f));
			m_MaxBt->BindOnClick([&]()
			{
				::ShowWindow(GetOwnerWindow()->GetRootWindowHandle(), SW_MAXIMIZE);
				GetOwnerWindow()->GetTransform()->SetLocalPosition(0, 0);
			});
		}
	}
	else
	{
		if (m_MaxBt)
		{
			canvas->DestroyJGUIElement(m_MaxBt->GetOwner());
			m_MaxBt = nullptr;
		}
	}
	// 최소화 버튼 활성화
	if (win_flags & JGUI_WindowFlag_EnableMinimize)
	{
		if (m_MinBt == nullptr)
		{
			auto min_bt_object = canvas->CreateJGUIElement("MinBt");
			min_bt_object->CreateJGUIComponent<JGUIImageRenderer>();
			m_MinBt = min_bt_object->CreateJGUIComponent<Button>();
			m_MinBt->SetImage("Minimize.png");
			m_MinBt->SetColor(Button::Pressed, JColor(0.8f, 0.3f, 0.3f, 1.0f));
			m_MinBt->SetColor(Button::Hightlight, JColor(1.0f, 0.0f, 0.0f, 1.0f));
			m_MinBt->BindOnClick([&]()
			{
				::ShowWindow(GetOwnerWindow()->GetRootWindowHandle(), SW_MINIMIZE);
			});
		}
	}
	else
	{
		if (m_MinBt)
		{
			canvas->DestroyJGUIElement(m_MinBt->GetOwner());
			m_MinBt = nullptr;
		}
	}
	Setting();
}

void TitleBar::Setting()
{
	auto size = GetOwner()->GetTransform()->GetSize();

	float pos    = size.x;
	float btsize = size.y;
	float gap = JGUI::Gap();


	if (m_CloseBt)
	{
		m_CloseBt->GetTransform()->SetSize(btsize, btsize);
		pos -= (btsize + gap);
		m_CloseBt->GetTransform()->SetLocalPosition(pos, 0);
	}
	if (m_MaxBt)
	{
		m_MaxBt->GetOwner()->GetTransform()->SetSize(btsize, btsize);
		pos -= (btsize + gap);
		m_MaxBt->GetTransform()->SetLocalPosition(pos, 0);
	}
	if (m_MinBt)
	{
		m_MinBt->GetTransform()->SetSize(btsize, btsize);
		pos -= (btsize + gap);
		m_MinBt->GetTransform()->SetLocalPosition(pos, 0);
	}
}

void TitleBar::MouseBtDown(const JGUIKeyDownEvent& e)
{
	auto win_flag = GetOwnerWindow()->GetWindowFlags();
	if (!m_IsGrap && e.Code == KeyCode::LeftMouseButton && (win_flag & JGUI_WindowFlag_NoMove) == false)
	{
		if (GetOwnerWindow()->GetParent())
		{
			// Dock에서 삭제
			DockSystem* dockSys = GetOwnerWindow()->GetParent()->GetDockSystem();
			if (dockSys)
			{
				dockSys->RequestRemoveBinedDock(GetOwnerWindow());
			}
			//
			GetOwnerWindow()->SetParent(nullptr);
			if (GetOwnerWindow()->GetParent())
			{
				GetOwnerWindow()->GetParent()->SetFocusWindow(GetOwnerWindow());
			}
		}
		m_IsGrap = true;


		auto mouse_pos = JGUI::GetCursorPos();
		JVector2 window_pos;
		window_pos = GetOwnerWindow()->GetTransform()->GetPosition();
		m_Delta.x = (int)window_pos.x - mouse_pos.x;
		m_Delta.y = (int)window_pos.y - mouse_pos.y;
		JGUI::RegisterDraggingWindow(GetOwnerWindow());


		JGUIExtraEvent extra_e;
		extra_e.Bind(JGUI_EXTRAEVENT_REPEAT, GetOwner(), [&](JGUIExtraEvent& e)
		{
			if (m_IsGrap && !m_IsUnConnect)
			{
				auto mouse_pos = JGUI::GetCursorPos();
				mouse_pos.x += m_Delta.x;
				mouse_pos.y += m_Delta.y;
				JVector2 pos((float)mouse_pos.x, (float)mouse_pos.y);

				if (GetOwnerWindow()->GetParent())
				{
					auto parent_pos = GetOwnerWindow()->GetParent()->GetTransform()->GetPosition();

					pos.x -= parent_pos.x;
					pos.y -= parent_pos.y;
				}
				GetOwnerWindow()->GetTransform()->SetLocalPosition(pos);
			}

			if (JGUI::GetKeyUp(GetOwnerWindow(), KeyCode::LeftMouseButton) || m_IsUnConnect)
			{
				if (m_IsGrap)
				{
					JGUI::RegisterDraggingWindow(nullptr);
					m_IsGrap = false;
					e.flag = JGUI_EXTRAEVENT_EXIT;
				}
				if (m_IsUnConnect) m_IsUnConnect = false;
			}
		});
		JGUI::RegisterExtraEvent(extra_e, 1);
	}
}
