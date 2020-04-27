#include "pch.h"
#include "JGUIWindow.h"

//#include "JGUIElement.h"

// Common Component Include
#include "GUI/JGUIComponent/JGUICamera.h"


// Element Component Include
#include "GUI/JGUIComponent/Element/JGUIRenderer.h"
#include "GUI/JGUIComponent/Element/JGUICanvas.h"

// Script Include
#include "GUI/JGUIComponent/Script/WindowSizeAdjustment.h"
#include "GUI/JGUIComponent/Script/TitleBar.h"
#include "GUI/JGUIComponent/Script/DockSystem.h"
#include "GUI/JGUIComponent/Script/TabSystem.h"
#include "GUI/JGUIComponent/Script/Scrollbar.h"
#include "GUI/JGUIComponent/Script/MainMenu.h"
#include "GUI/JGUIComponent/Script/MenuItem.h"

// RenderEngine Include
#include "GUI/JGUIScreen.h"
#include "RenderEngine.h"
#include "Object/DxObject/RenderDevice.h"
using namespace std;
using namespace RE;







void JGUIWindow::JGUIAwake()
{
	// Transform 생성 및 충돌박스 등록
	m_Transform = CreateJGUIComponent<JGUITransform>();
	RegisterCollider(JGUI_Collider_Box);

	auto transform = GetTransform();
	auto window_size = transform->GetSize();

	// 3. Canvas 생성
	{
		m_RootElement = JGUI::CreateJGUIElement<JGUIElement>(GetName() + "Root", this, JGUI_ComponentFlag_None);

		m_EssentialCanvas = JGUI::CreateJGUIElement<JGUIElement>(GetName() + "Canvas", this, JGUI_ComponentFlag_None);
		auto canvas = m_EssentialCanvas->CreateJGUIComponent<JGUICanvas>();
		canvas->SetCanvasFlags(JGUI_CanvasFlag_TopMost | JGUI_CanvasFlag_Overlay);

		m_ElementCanvas = JGUI::CreateJGUIElement<JGUIElement>("ElementCanvas", this, JGUI_ComponentFlag_None);
		m_ElementCanvas->CreateJGUIComponent<JGUICanvas>();

		m_ElementCanvas->SetParent(m_RootElement);
		m_EssentialCanvas->SetParent(m_RootElement);
		
	
		m_RootElement->GetTransform()->AttachTransform(m_ElementCanvas->GetTransform());
		m_RootElement->GetTransform()->AttachTransform(m_EssentialCanvas->GetTransform());
		m_WindowElements.push_back(m_RootElement);
	}

	// 4. 윈도우 텍스쳐 생성
	m_WinRenderer = CreateJGUIElement("WinRenderer")->CreateJGUIComponent<JGUIWindowRenderer>();
	m_WinRenderer->GetTransform()->SetSize(GetTransform()->GetSize());


	// 5. 윈도우 카메라 준비
	ReadyCamera();


	// 6. FLAG 에 따른 윈도우 제작
	if (m_Flags & JGUI_WindowFlag_NewLoad)
	{
		SetParent(nullptr);
	}

	ProcessByWindowFlags(m_Flags);
	JGUIObject::JGUIAwake();
}

void JGUIWindow::JGUIStart()
{
	JGUIObject::JGUIStart();

	for (size_t i = 0; i < m_WindowElements.size(); ++i)
	{
		if (m_WindowElements[i]->IsExecuteStartFunc()) continue;
		m_WindowElements[i]->JGUIStart();
	}
	for (size_t i = 0; i < m_ChildWindows.size(); ++i)
	{
		if (m_ChildWindows[i]->IsExecuteStartFunc()) continue;
		m_ChildWindows[i]->JGUIStart();
	}
}


void JGUIWindow::JGUIDestroy()
{
	JGUIObject::JGUIDestroy();
	RE::RenderEngine::GetDevice()->Flush();


	// 부모 윈도우 배열에서 삭제
	if (m_ParentWindow)
	{
		auto& childs = m_ParentWindow->m_ChildWindows;
		if (m_ParentWindow->GetFocusWindow() == this) m_ParentWindow->SetFocusWindow(nullptr);
		childs.erase(std::remove(childs.begin(), childs.end(), this), childs.end());
	}

	vector<JGUIWindow*> child_clones = m_ChildWindows;

	// 자식들 손절
	for (auto& window : child_clones)
	{
		JGUI::DestroyObject(window);
		if (GetFocusWindow() == window) SetFocusWindow(nullptr);
	}
	// 컴포넌트 삭제
	for (auto& element : m_WindowElements)
	{
		JGUI::DestroyObject(element);
		if (GetFocusElement() == element) SetFocusElement(nullptr);
	}
	// 스크린 삭제 요청
	if (m_Screen)
	{
		JGUI::RequestDestroyScreen(m_Screen->GetHandle());
	}

	SetActive(false);
}

void JGUIWindow::JGUITick(const JGUITickEvent& e)
{

	JGUIObject::JGUITick(e);

	if (GetFocusElement())   GetFocusElement()->JGUIOnFocus();
	if (GetFocusWindow())    GetFocusWindow()->JGUIOnFocus();

	JGUIElement::RISortingOrderReset();

	if (GetTransform()->IsDirty())
	{
		m_WinRenderer->GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition());
	}
	for (size_t i = 0; i < m_WindowElements.size(); ++i)
	{
		if (!m_WindowElements[i]->IsExecuteStartFunc()) m_WindowElements[i]->JGUIStart();
		if (!m_WindowElements[i]->IsActive()) continue;
		if (m_WindowElements[i]->IsDestroying()) continue;
		m_WindowElements[i]->JGUITick(e);
	}
	for (size_t i = 0; i < m_ChildWindows.size(); ++i)
	{
		if (!m_ChildWindows[i]->IsExecuteStartFunc()) m_ChildWindows[i]->JGUIStart();
		if (!m_ChildWindows[i]->IsActive()) continue;
		if (m_ChildWindows[i]->IsDestroying()) continue;
		m_ChildWindows[i]->JGUITick(e);
	}
}

void JGUIWindow::JGUIResize(const JGUIResizeEvent& e)
{
	float width  = e.width;
	float height = e.height;
	if (e.width < 200 || e.height < 200)
	{
		width  = std::max<float>(e.width, 200);
		height = std::max<float>(e.height, 200);
		if (GetParent())
		{
			GetTransform()->SetSize(width, height);
			JGUIResizeEvent ee; ee.width = width; ee.height = height;
			JGUIResize(ee);
		}
		else
		{
			GetTransform()->SetSize(width, height);
		}
	}

	m_WinRenderer->GetTransform()->SetSize(width, height);

	m_RootElement->GetTransform()->SetSize(width, height);
	
	RE::RenderEngine::GetDevice()->Flush();
	ReadyCamera();
	
	if (m_Screen)
	{
		m_Screen->Resize((uint32_t)width, (uint32_t)height);
		m_Screen->BindGUIModuleClone(m_WinCamera->GetShaderModule());
	}
	for (size_t i = 0; i < m_ChildWindows.size(); ++i)
	{
		m_ChildWindows[i]->GetTransform()->SendDirty();
	}
	{
		float offsety = 0.0f;
		float thickness = 0.0f;

		if (m_MainMenu)
		{
			m_MainMenu->GetTransform()->SetSize(width, 20);
			offsety += 20.0f;
		}
		if (m_TitleBar)
		{
			float spare_width = 0.0f;
			m_TitleBar->GetTransform()->SetSize(width, m_TitleSize);
			if (m_Flags & JGUI_WindowFlag_EnableClose)    spare_width += m_TitleSize;
			if (m_Flags & JGUI_WindowFlag_EnableMaximize) spare_width += m_TitleSize;
			if (m_Flags & JGUI_WindowFlag_EnableMinimize) spare_width += m_TitleSize;
			m_TabSystem->GetTransform()->SetSize(width - spare_width, m_TitleSize);
			offsety += m_TitleSize;
		}
		if (m_WinSizeAdjment)
		{
			m_WinSizeAdjment->PositionAdjustment();

		}
		if (m_Border)
		{
			m_Border->GetTransform()->SetSize(GetTransform()->GetSize());
			thickness = m_Border->GetThickness();
			offsety += thickness;
		}
		if (m_DockSystem)
		{
			auto client_rect = GetClientRect();
			m_DockSystem->GetTransform()->SetLocalPosition(client_rect.left, client_rect.top);
			m_DockSystem->GetTransform()->SetSize(client_rect.width(), client_rect.height());
		}
		if (m_VScrollbar)
		{
			float scrollheight = height - offsety - (thickness * 2);
			if (m_HScrollbar) scrollheight -= 20;
			else if (m_WinSizeAdjment) scrollheight -= m_ResizeBoxSize;
			m_VScrollbar->GetTransform()->SetSize(20, scrollheight);
			m_VScrollbar->GetTransform()->SetLocalPosition(width - 20 - thickness, offsety + 2);
		}
		if (m_HScrollbar)
		{
			float scrollwidth = width - thickness;
			if (m_VScrollbar) scrollwidth -= 20;
			else if (m_WinSizeAdjment) scrollwidth -= m_ResizeBoxSize;
			m_HScrollbar->GetTransform()->SetSize(scrollwidth, 20);
			m_HScrollbar->GetTransform()->SetLocalPosition(thickness, height - 20 - thickness);
		}
	}
	Resize(e);
}
void JGUIWindow::JGUIFocusEnter(const JGUIFocusEnterEvent& e)
{

	FocusEnter(e);
}
void JGUIWindow::JGUIFocusExit(const JGUIFocusExitEvent& e)
{
	JGUI::InputFlush();
	SetFocusElement(nullptr);
	SetFocusWindow(nullptr);
	FocusExit(e);
}
void JGUIWindow::JGUIOnFocus()
{
	OnFocus();
}

void JGUIWindow::JGUIMouseWheel(const JGUIMouseWheelEvent& e)
{
	if (e.delta == 0) return;
	if (GetFocusElement()) GetFocusElement()->JGUIMouseWheel(e);
	if (GetFocusWindow())    GetFocusWindow()->JGUIMouseWheel(e);
	else 
	{
		if (m_HScrollbar)
		{
			m_WinCamera->OffsetPosition(e.delta,0);
		}
		MouseWheel(e);
	}
}

void JGUIWindow::JGUIChar(const JGUICharEvent& e)
{
	if (GetFocusElement()) GetFocusElement()->JGUIChar(e);
	if (GetFocusWindow()) GetFocusWindow()->JGUIChar(e);
	else Char(e);
}

void JGUIWindow::JGUIKeyDown(const JGUIKeyDownEvent& e)
{
	if (GetFocusElement()) GetFocusElement()->JGUIKeyDown(e);
	if (GetFocusWindow()) GetFocusWindow()->JGUIKeyDown(e);
	else
	{
		if (m_MainMenu)
		{
			m_MainMenu->ReceiveKeyDown(e);
		}
	
		KeyDown(e);
	}
}
void JGUIWindow::JGUIKeyUp(const JGUIKeyUpEvent& e)
{
	if (GetFocusElement()) GetFocusElement()->JGUIKeyUp(e);
	if (GetFocusWindow()) GetFocusWindow()->JGUIKeyUp(e);
	else
	{
		if (m_MainMenu)
		{
			m_MainMenu->ReceiveKeyUp(e);
		}
		KeyUp(e);
	}

}

void JGUIWindow::JGUIMouseBtDown(const JGUIKeyDownEvent& e)
{
	// Component 와의 상호 작용
	JGUIElement* focus_com = m_RootElement;
	focus_com = focus_com->TrackingCanInteractionComponent();


	// Window 찾기
	JGUIWindow* focus_win = TrackingCanInteractionWindow();

	if (!m_IsMouseDown || m_IsMouseLeave)
	{
		if (focus_com && focus_com->GetElementFlags() & JGUI_ComponentFlag_TopMost)
		{
			if (focus_com == nullptr)
			{
				SetFocusWindow(focus_win);
			}
			else SetFocusWindow(nullptr);
			SetFocusElement(focus_com);

		}
		else
		{
			if (focus_win == nullptr) SetFocusElement(focus_com);
			else SetFocusElement(nullptr);

			SetFocusWindow(focus_win);
		}

		
		m_IsMouseDown = true;
		m_IsMouseLeave = false;
	}
	
	focus_win = GetFocusWindow();
	focus_com = GetFocusElement();

	if (focus_com) focus_com->JGUIMouseBtDown(e);
	if (focus_win) focus_win->JGUIMouseBtDown(e);
	else MouseBtDown(e);
}

void JGUIWindow::JGUIMouseBtUp(const JGUIKeyUpEvent& e)
{
	auto focus_com = GetFocusElement();
	auto focus_win = GetFocusWindow();




	if (focus_com && focus_com->Interation()) focus_com->JGUIMouseBtUp(e);
	if (focus_win && focus_win->m_IsMouseDown)
	{
		focus_win->JGUIMouseBtUp(e);
	}
	else
	{
	
		MouseBtUp(e);
	}



	if (m_IsMouseDown) m_IsMouseDown = false;

	
}
void JGUIWindow::JGUIMouseMove(const JGUIMouseMoveEvent& e)
{
	if (GetParent() && m_IsTracking ==  false)
	{
		m_IsTracking = true;
		JGUIMouseTrack track;
		track.win = this;
		track.time = 0.01f;
		track.flag = JGUI_MOUSETRACKFLAG_MOUSEHOVER;
		JGUI::RegisterMouseTrack(track);
	}
	JGUIWindow* top_window = TrackingCanInteractionWindow();
	if (top_window)
	{
		auto child_mouse_pos = top_window->GetMousePos();
		JGUIMouseMoveEvent child_e = e;
		child_e.pos.x = (float)child_mouse_pos.x;
		child_e.pos.y = (float)child_mouse_pos.y;
		top_window->JGUIMouseMove(child_e);
	}
	else
	{
		for (size_t i = 0; i < m_WindowElements.size(); ++i)
		{
			if (!m_WindowElements[i]->IsActive()) continue;
			if (m_WindowElements[i]->IsDestroying()) continue;
			m_WindowElements[i]->JGUIMouseMove(e);
		}
	}

	MouseMove(e);
}
void JGUIWindow::JGUIMouseHover()
{
	if (GetParent() && m_IsTracking)
	{
		JGUIMouseTrack track;
		track.win = this;
		track.time = 0.01f;
		track.flag = JGUI_MOUSETRACKFLAG_MOUSELEAVE;
		JGUI::RegisterMouseTrack(track);
	} 
	MouseHover();
}
void JGUIWindow::JGUIMouseLeave()
{
	if (GetParent() && m_IsTracking)
	{
		m_IsTracking = false;
	}
	m_IsMouseLeave = true;
	JGUI::InputMouseFlush();
	MouseLeave();
}


void JGUIWindow::SetParent(JGUIWindow* parent)
{
	auto prev_parent = m_ParentWindow;

	// 메인이 자기 자신이면 parent는 null
	if (JGUI::GetMainWindow() == this ||
		JGUI::GetMainWindow() == nullptr)
	{
		parent = nullptr;
	}
	else if (!(JGUI::GetMainWindow()->GetWindowFlags() & JGUI_WindowFlag_MultiSwapChain) && parent == nullptr)
	{
		parent = JGUI::GetMainWindow();
	}


	if (m_ParentWindow != nullptr && m_ParentWindow == parent) return;



	if (parent == nullptr) // 최상단 루트가 되어라
	{
		// 기존 부모 윈도우의 포커스를 풀고, 자식 배열에서 삭제 및 위치 조정
		if (m_ParentWindow)
		{
			if (this == m_ParentWindow->GetFocusWindow()) m_ParentWindow->SetFocusWindow(nullptr);
			
			auto remove_iter = remove(m_ParentWindow->m_ChildWindows.begin(), m_ParentWindow->m_ChildWindows.end(), this);
			m_ParentWindow->m_ChildWindows.erase(remove_iter, m_ParentWindow->m_ChildWindows.end());



			GetTransform()->SendPosDirty();
			auto pos = GetTransform()->GetPosition();
			GetTransform()->SetLocalPosition(pos);
		}
		m_ParentWindow = nullptr;
		m_Priority = JGUI_WindowPriority_None;
		//
		if (m_Screen == nullptr)
		{
			NewLoad();
		}
	}
	else
	{
		// Screen이 있다면 삭제
		if (m_Screen)
		{
			JGUI::RequestDestroyScreen(m_Screen->GetHandle());
			m_Screen = nullptr;
		}
		JGUIWindow* prev_parent_window = nullptr;
		// 기존 부모 윈도우의 포커스를 풀고, 자식 배열에서 삭제 및 위치 조정
		if (m_ParentWindow)
		{
			prev_parent_window = m_ParentWindow;
			if (this == m_ParentWindow->GetFocusWindow()) m_ParentWindow->SetFocusWindow(nullptr);

			auto remove_iter = remove(m_ParentWindow->m_ChildWindows.begin(), m_ParentWindow->m_ChildWindows.end(), this);
			m_ParentWindow->m_ChildWindows.erase(remove_iter, m_ParentWindow->m_ChildWindows.end());
		}

		m_Priority = parent->ChildWindowSortByPriority();
		parent->m_ChildWindows.push_back(this);
		GetTransform()->SendPosDirty();
		auto pos = GetTransform()->GetPosition();
		
		//
		m_ParentWindow = parent;
		//
		auto parent_pos = parent->GetTransform()->GetPosition();
		pos = pos - parent_pos;
		GetTransform()->SetLocalPosition(pos);

		m_WinRenderer->SetImage(m_WinCamera->GetTextureCacheKey());
	}

	NotifyChangeParentToComponents(prev_parent);

}
JGUIWindow* JGUIWindow::GetParent() const
{
	return m_ParentWindow;
}

const std::vector<JGUIWindow*>& JGUIWindow::GetChilds() const
{
	return m_ChildWindows;
}
std::vector<JGUIWindow*>& JGUIWindow::GetChilds()
{
	return m_ChildWindows;
}

std::vector<JGUIElement*>& JGUIWindow::GetWindowElements()
{
	return m_WindowElements;
}
JGUIWindow* JGUIWindow::FindChild(uint32_t index)
{
	if (m_ChildWindows.size() <= index)
		return nullptr;
	return m_ChildWindows[index];
}
JGUIWindow* JGUIWindow::FindChild(const std::string& name)
{
	std::find_if(m_ChildWindows.begin(), m_ChildWindows.end(), [&](JGUIWindow* window) -> bool
	{
		return window->GetName() == name;
	});

	return nullptr;
}
void JGUIWindow::DestroyJGUIWindow(JGUIWindow* window)
{
	JGUI::DestroyObject(window);
	if (window == GetFocusWindow()) SetFocusWindow(nullptr);
} 
void JGUIWindow::DestroyJGUIElement(JGUIElement* com)
{
	JGUI::DestroyObject(com);
	if (com == GetFocusElement()) SetFocusElement(nullptr);
}

JVector2Int JGUIWindow::GetMousePos()
{
	if (GetParent() == nullptr)
	{
		JVector2Int result = JGUI::GetMousePos(GetRootWindowHandle());
		return result;
	}

	auto window_pos = GetTransform()->GetPosition();
	auto mouse_pos = JGUI::GetCursorPos();

	JVector2Int pos;
	pos.x = mouse_pos.x - window_pos.x;
	pos.y = mouse_pos.y - window_pos.y;
	return pos;
}
HWND JGUIWindow::GetRootWindowHandle() const
{
	if (m_Screen)
		return m_Screen->GetHandle();
	else if (m_ParentWindow)
		return m_ParentWindow->GetRootWindowHandle();
	else
		return 0;
}

void JGUIWindow::SetFocusElement(JGUIElement* com)
{
	if (m_FocusElement != com)
	{
		if (m_FocusElement) // exit
		{
			JGUIFocusExitEvent exit_e;
			exit_e.nextFocus = com;
			m_FocusElement->JGUIFocusExit(exit_e);
		}
		JGUIFocusEnterEvent enter_e;
		enter_e.prevFocus = m_FocusElement;

		//
		m_FocusElement = com;
		if (com) // enter
		{
			com->JGUIFocusEnter(enter_e);
		}
	}
}
JGUIElement* JGUIWindow::GetFocusElement()
{
	return m_FocusElement;
}

void JGUIWindow::SetFocusWindow(JGUIWindow* win)
{

	if (m_FocusWindow != win)
	{
		if (m_FocusWindow)
		{
			JGUIFocusExitEvent exit_e;
			exit_e.nextFocus = win;
			m_FocusWindow->JGUIFocusExit(exit_e);

		}
		JGUIFocusEnterEvent enter_e;
		enter_e.prevFocus = m_FocusWindow;


		auto p = ChildWindowSortByPriority();
		if (m_FocusWindow && win == nullptr) m_FocusWindow->m_Priority = p;
		m_FocusWindow = win;
		if (win)
		{
			win->JGUIFocusEnter(enter_e);
			win->m_Priority = JGUI_WindowPriority_Focus;
		}
	}
}

JGUIWindow* JGUIWindow::GetFocusWindow()
{
	return m_FocusWindow;
}



void JGUIWindow::SetPriority(EJGUI_WindowPriority p)
{
	m_Priority = p;
}
void JGUIWindow::SetWindowFlags(EJGUI_WindowFlags flag)
{
	m_Flags = flag;
	ProcessByWindowFlags(m_Flags);
}
void JGUIWindow::AddWindowFlags(EJGUI_WindowFlags flag)
{
	m_Flags = m_Flags | flag;
	ProcessByWindowFlags(m_Flags);
}
void JGUIWindow::RemoveWindowFlags(EJGUI_WindowFlags flag)
{
	m_Flags = m_Flags & (~flag);
	ProcessByWindowFlags(m_Flags);
}
EJGUI_WindowFlags JGUIWindow::GetWindowFlags() const
{
	return m_Flags;
}
void JGUIWindow::SetActive(bool active, bool is_hierarchy)
{
	JGUIObject::SetActive(active, is_hierarchy);
	for (auto& com : m_WindowElements)
	{
		com->SetActive(active, is_hierarchy);
	}
	for (auto& win : m_ChildWindows)
	{
		win->SetActive(IsActive(), true);
	}
}
JGUIRect JGUIWindow::GetClientRect() const
{
	JGUIRect rect = m_RootElement->GetTransform()->GetLocalRect();

	if (m_Border)
	{
		float thickness = m_Border->GetThickness();
		rect.top += thickness;
		rect.left += thickness;
		rect.bottom -= thickness;
		rect.right -= thickness;
	}
	if (m_TitleBar)
	{
		auto title_size = m_TitleBar->GetTransform()->GetSize();
        rect.top += title_size.y;
	}
	if (m_MainMenu)
	{
		auto menu_size = m_MainMenu->GetTransform()->GetSize();
		rect.top += menu_size.y;
	}
	if (m_VScrollbar)
	{
		auto scroll_size = m_VScrollbar->GetTransform()->GetSize();
		rect.right -= scroll_size.x;

	}
	if (m_HScrollbar)
	{
		auto scroll_size = m_HScrollbar->GetTransform()->GetSize();
		rect.bottom -= scroll_size.y;
	}
	return rect;
}

void JGUIWindow::SetActiveExceptEssentials(bool is_active)
{

	for (int i = 0; i < m_ElementCanvas->GetChildCount(); ++i)
	{
		auto com = m_ElementCanvas->GetChild(i);
		if (m_WinRenderer->GetOwner() == com) continue;
		com->SetActive(is_active, true);
	}
	if (m_DockSystem)
	{
		m_DockSystem->GetOwner()->SetActive(is_active, true);
	}
}
void JGUIWindow::ProcessByWindowFlags(EJGUI_WindowFlags flag)
{
	auto com_flag = JGUI_ComponentFlag_Overlay;
	if (flag & JGUI_WindowFlag_MultiSwapChain) com_flag = JGUI_ComponentFlag_TopMost;
	if (flag & JGUI_WindowFlag_EnableResize)
	{
		if (m_WinSizeAdjment == nullptr)
		{
			auto obj = CreateJGUIEssentialElement<JGUIElement>("ResizeBox", JGUI_ComponentFlag_TopMost);
			obj->CreateJGUIComponent<JGUIImageRenderer>();
			m_WinSizeAdjment = obj->CreateJGUIComponent<WindowSizeAdjustment>();
		}
	}
	else
	{
		if (m_WinSizeAdjment)
		{
			DestroyJGUIElement(m_WinSizeAdjment->GetOwner());
			m_WinSizeAdjment = nullptr;
		}
	}
	if (flag & JGUI_WindowFlag_EnableDock)
	{
		if (m_DockSystem == nullptr)
		{
			m_DockSystem = CreateJGUIElement("DockSystem")->CreateJGUIComponent<DockSystem>();
		}
	}
	else
	{
		if (m_DockSystem)
		{
			DestroyJGUIElement(m_DockSystem->GetOwner());
			m_DockSystem = nullptr;
		}
	}
	// TitleBar
	if (flag & JGUI_WindowFlag_TitleBar)
	{
		if (m_TitleBar == nullptr)
		{
			m_TitleBar = CreateJGUIEssentialElement<JGUIElement>("TitleBar", com_flag)->CreateJGUIComponent<TitleBar>();
			m_TabSystem = CreateJGUIEssentialElement<JGUIElement>("TabSystem", com_flag)->CreateJGUIComponent<TabSystem>();
		}
	}
	else
	{
		if (m_TitleBar)
		{
			DestroyJGUIElement(m_TitleBar->GetOwner());
			DestroyJGUIElement(m_TabSystem->GetOwner());
			m_TitleBar = nullptr;
			m_TabSystem = nullptr;
		}
	}

	// Menu
	if (flag & JGUI_WindowFlag_MenuBar)
	{
		if (m_MainMenu == nullptr)
		{
			m_MainMenu = CreateJGUIEssentialElement<JGUIElement>("JGUIMenu", com_flag)->CreateJGUIComponent<MainMenu>();
		}
	}
	else
	{
		if (m_MainMenu)
		{
			DestroyJGUIElement(m_MainMenu->GetOwner());
			m_MainMenu = nullptr;
		}
	}

	// HScrollBar
	if (flag & JGUI_WindowFlag_HorizontalScrollbar)
	{
		if (m_HScrollbar == nullptr)
		{
			m_HScrollbar = CreateJGUIEssentialElement<JGUIElement>("HScrollbar")->CreateJGUIComponent<Scrollbar>();
			m_HScrollbar->SetBarType(Scrollbar::Horizontal);
		}
	}
	else
	{
		if (m_HScrollbar)
		{
			DestroyJGUIElement(m_HScrollbar->GetOwner());
			m_HScrollbar = nullptr;
		}
	}
	if (flag & JGUI_WindowFlag_VerticalScrollbar)
	{
		if (m_VScrollbar == nullptr)
		{
			m_VScrollbar = CreateJGUIEssentialElement<JGUIElement>("VScrollbar")->CreateJGUIComponent<Scrollbar>();
			m_VScrollbar->SetBarType(Scrollbar::Vertical);
		}
	}
	else
	{
		if (m_VScrollbar)
		{
			DestroyJGUIElement(m_VScrollbar->GetOwner());
			m_VScrollbar = nullptr;
		}
	}
	if (flag & JGUI_WindowFlag_Border)
	{
		if (m_Border == nullptr)
		{
			m_Border = CreateJGUIEssentialElement<JGUIElement>("BorderRectangle", com_flag)->CreateJGUIComponent<JGUIShapeRenderer>();

			m_Border->GetTransform()->SetSize(GetTransform()->GetSize());
			m_Border->SetColor(JColor(1, 1, 1, 1));
			m_Border->CreateEmptyRectangle(1.0);
		}

	}
	else
	{
		if (m_Border)
		{
			DestroyJGUIElement(m_Border->GetOwner());
			m_Border = nullptr;
		}
	}


	float offset_x = 0.0f;
	float offset_y = 0.0f;

	if (m_Border)
	{
		offset_x = 1.5f;
		offset_y = 1.5f;
	}

	JVector2 window_size = GetTransform()->GetSize();

	if (m_TitleBar)
	{
		float title_width = window_size.x - (2 * offset_x);
		float spare_width = 0.0f;
		m_TitleBar->GetTransform()->SetLocalPosition(offset_x, offset_y);
		m_TitleBar->GetTransform()->SetSize(title_width, m_TitleSize);


		if (m_Flags & JGUI_WindowFlag_EnableClose)    spare_width += m_TitleSize;
		if (m_Flags & JGUI_WindowFlag_EnableMaximize) spare_width += m_TitleSize;
		if (m_Flags & JGUI_WindowFlag_EnableMinimize) spare_width += m_TitleSize;

		m_TabSystem->GetTransform()->SetLocalPosition(offset_x, offset_y);
		m_TabSystem->GetTransform()->SetSize(title_width - spare_width, m_TitleSize);
		offset_y += 20;
	}
	if (m_MainMenu)
	{
		m_MainMenu->GetTransform()->SetLocalPosition(offset_x, offset_y);
		m_MainMenu->GetTransform()->SetSize(window_size.x - (2 * offset_x), 20);
		offset_y += 20;
	}
	if (m_VScrollbar)
	{
		float thickness = m_Border->GetThickness();
		float scroll_size_y = window_size.y - (20 * 2 + thickness * 2);
		if (m_HScrollbar) scroll_size_y -= 20;
		else if (m_WinSizeAdjment) scroll_size_y -= m_ResizeBoxSize;
		m_VScrollbar->GetTransform()->SetSize(m_ResizeBoxSize, scroll_size_y);
		m_VScrollbar->GetTransform()->SetLocalPosition(window_size.x - m_ResizeBoxSize - thickness, offset_y);
	}

	if (m_HScrollbar)
	{
		float thickness = m_Border->GetThickness();
		float scroll_size_x = window_size.x - (thickness * 2);
		if (m_VScrollbar) scroll_size_x -= 20;
		else if (m_WinSizeAdjment) scroll_size_x -= 20;
		m_HScrollbar->GetTransform()->SetSize(window_size.x - (thickness * 2), 20);
		m_HScrollbar->GetTransform()->SetLocalPosition(offset_x, window_size.y - m_ResizeBoxSize - thickness);
	}
	if (m_DockSystem)
	{
		auto client_rect = GetClientRect();
		m_DockSystem->GetTransform()->SetLocalPosition(client_rect.left, client_rect.top);
		m_DockSystem->GetTransform()->SetSize(client_rect.width(), client_rect.height());
	}
	if (m_WinSizeAdjment)
	{
		m_WinSizeAdjment->GetOwner()->GetTransform()->SetSize(m_ResizeBoxSize, m_ResizeBoxSize);
	}
	
}




void JGUIWindow::NewLoad()
{
	if (m_Screen == nullptr)
	{
		m_Screen = JGUI::ReqeustRegisterJGUIScreen(this);
	}
	if (m_Screen)
	{
		m_Screen->BindGUIModuleClone(m_WinCamera->GetShaderModule());
	}
}

void JGUIWindow::ReadyCamera()
{
	auto size = GetTransform()->GetSize();
	if(m_WinCamera == nullptr)
	{ 
		m_WinCamera = CreateJGUIComponent<JGUICamera>();
	}
	m_WinCamera->SetLens(size.x, size.y);
	if (GetParent())
	{
		m_WinRenderer->SetImage(m_WinCamera->GetTextureCacheKey());
	}
}
bool JGUIWindow::Interaction()
{
	if (GetParent() == nullptr) return true;

	return IsActive() && GetCollider() && GetCollider()->CheckInPoint(GetParent()->GetMousePos());
}

void JGUIWindow::NotifyChangeParentToComponents(JGUIObject* prevparent)
{
	for (auto& com : m_Coms)
	{
		com->JGUIChangeParent(prevparent);
	}

	for (auto& win : m_ChildWindows)
	{
		win->NotifyChangeParentToComponents(prevparent);
	}
	for (auto& element : m_WindowElements)
	{
		element->NotifyChangeParentToComponents(prevparent);
	}


}

EJGUI_WindowPriority JGUIWindow::ChildWindowSortByPriority()
{
	// 부모가 될 윈도우 우선순위 재정렬 & 재정비
	std::sort(m_ChildWindows.begin(),m_ChildWindows.end(),
		[](JGUIWindow* win1, JGUIWindow* win2)
	{
		return win1->GetPriority() < win2->GetPriority();
	});
	EJGUI_WindowPriority p = JGUI_WindowPriority_Child;
	for (auto& child : m_ChildWindows)
	{
		child->m_Priority = p;
		p = (EJGUI_WindowPriority)(p + 1);
	}
	return p;
}

JGUIWindow* JGUIWindow::TrackingCanInteractionWindow()
{
	JGUIWindow* result = nullptr;
	for (auto& child : m_ChildWindows)
	{
		if (child->Interaction())
		{
			if(result == nullptr) result = child;
			else
			{
				if (result->GetPriority() < child->GetPriority()) result = child;
			}
		}
	}

	return result;
}


