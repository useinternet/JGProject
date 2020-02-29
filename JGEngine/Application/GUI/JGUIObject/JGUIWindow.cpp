#include "pch.h"
#include "JGUIWindow.h"
#include "JGUIComponent.h"

// JGUICompoennts Include
#include "GUI/JGUIComponent/JGUIRectTransform.h"
#include "GUI/JGUIComponent/JGUIPanel.h"
#include "GUI/JGUIComponent/JGUICollider.h"
#include "GUI/JGUIComponent/JGUIShape.h"
#include "GUI/JGUIComponent/JGUITitleBar.h"
#include "GUI/JGUIComponent/JGUIResizeBox.h"




// RenderEngine Include
#include "GUI/JGUIScreen.h"
#include "RenderEngine.h"

#include "Object/Shader/ShaderModule.h"
#include "Object/ReObject/RenderItem.h"
#include "Object/DxObject/RenderDevice.h"
#include "Object/ReObject/ReMesh.h"
#include "Object/ReObject/ReCamera.h"
#include "Object/ReObject/ReMaterial.h"
using namespace std;
using namespace RE;







void JGUIWindow::JGUIAwake()
{


	JGUIObject::JGUIAwake();
}

void JGUIWindow::JGUIStart()
{
	for (auto& com : m_WindowComponents)
	{
		if (com->IsExecuteStartFunc()) continue;
		com->JGUIStart();
	}
	for (auto& window : m_ChildWindows)
	{
		if (window->IsExecuteStartFunc()) continue;
		window->JGUIStart();
	}
	JGUIObject::JGUIStart();


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
	for (auto& com : m_WindowComponents)
	{
		JGUI::DestroyObject(com);
		if (GetFocusComponent() == com) SetFocusComponent(nullptr);
	}

	RE::RenderEngine::UnRegisterRIManager(GetID());

	if (m_GUIModule)
	{
		RE::RenderEngine::UnRegisterTexture(m_GUIModule->GetTextureCacheKey());
		m_WinTexture->UnBind();
	}
	m_GUIModule = nullptr;
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
	
	if (GetFocusComponent()) GetFocusComponent()->JGUIOnFocus();
	if (GetFocusWindow())    GetFocusWindow()->JGUIOnFocus();

	JGUIComponent::RISortingOrderReset();
	for (auto& com : m_WindowComponents)
	{
		if (!com->IsExecuteStartFunc()) com->JGUIStart();
		if (!com->IsActive()) continue;
		com->JGUITick(e);
	}
	for (auto& window : m_ChildWindows)
	{
		if (!window->IsExecuteStartFunc()) window->JGUIStart();
		if (!window->IsActive()) continue;
		window->JGUITick(e);
	}
	auto device = RE::RenderEngine::GetDevice();
	if (m_GUIModule)
	{
		device->SubmitToRender(JGUI_SubmitCmdListPriority_Default, [&](RE::CommandList* cmdList)
		{
			if (m_GUIModule == nullptr) return;


			m_GUIModule->Execute(cmdList);

		});
	}
}

void JGUIWindow::JGUIResize(const JGUIResizeEvent& e)
{
	if (GetParent() == nullptr)
	{
		GetTransform()->SetSize(e.width, e.height);
	}



	m_Panel->GetTransform()->SetSize(e.width, e.height);
	m_Panel->SetColor({ 1.0f,0.0f,1.0f,0.3f });
	RE::RenderEngine::GetDevice()->Flush();
	ReadyGUIModule();
	
	if (m_Screen)
	{
		m_Screen->Resize((uint32_t)e.width, (uint32_t)e.height);
		m_Screen->BindGUIModuleClone(m_GUIModule.get());
	}

	for (auto& child : m_ChildWindows)
	{
		child->GetTransform()->SendDirty();
	}



	Resize(e);
	// resize 구현
}
void JGUIWindow::JGUIFocusEnter(const JGUIFocusEnterEvent& e)
{

	FocusEnter(e);
}
void JGUIWindow::JGUIFocusExit(const JGUIFocusExitEvent& e)
{
	JGUI::InputFlush();
	SetFocusComponent(nullptr);
	SetFocusWindow(nullptr);
	FocusExit(e);
}
void JGUIWindow::JGUIOnFocus()
{
	OnFocus();
}

void JGUIWindow::JGUIChar(const JGUICharEvent& e)
{
	if (GetFocusComponent()) GetFocusComponent()->JGUIChar(e);
	if (GetFocusWindow()) GetFocusWindow()->JGUIChar(e);
	else Char(e);
}

void JGUIWindow::JGUIKeyDown(const JGUIKeyDownEvent& e)
{
	if (GetFocusComponent()) GetFocusComponent()->JGUIKeyDown(e);
	if (GetFocusWindow()) GetFocusWindow()->JGUIKeyDown(e);
	else
	{
	
		KeyDown(e);
	}
}
void JGUIWindow::JGUIKeyUp(const JGUIKeyUpEvent& e)
{
	if (GetFocusComponent()) GetFocusComponent()->JGUIKeyUp(e);
	if (GetFocusWindow()) GetFocusWindow()->JGUIKeyUp(e);
	else
	{
		
		KeyUp(e);
	}

}

void JGUIWindow::JGUIMouseBtDown(const JGUIKeyDownEvent& e)
{
	// Component 와의 상호 작용
	JGUIComponent* focus_com = m_Panel;
	focus_com = focus_com->TrackingCanInteractionComponent();
	// Window 찾기
	JGUIWindow* focus_win = TrackingCanInteractionWindow();



	if (!m_IsMouseDown || m_IsMouseLeave)
	{
		if(focus_win == nullptr) SetFocusComponent(focus_com);
		else SetFocusComponent(nullptr);

		SetFocusWindow(focus_win);
		m_IsMouseDown = true;
		m_IsMouseLeave = false;
	}

	focus_win = GetFocusWindow();
	focus_com = GetFocusComponent();




	if (focus_com) focus_com->JGUIMouseBtDown(e);

	if (focus_win) focus_win->JGUIMouseBtDown(e);
	else MouseBtDown(e);
}

void JGUIWindow::JGUIMouseBtUp(const JGUIKeyUpEvent& e)
{
	auto focus_com = GetFocusComponent();
	auto focus_win = GetFocusWindow();




	if (focus_com && focus_com->Interation()) focus_com->JGUIMouseBtUp(e);
	if (focus_win && focus_win->Interaction())
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
	for (auto& com : m_WindowComponents)
	{
		com->JGUIMouseMove(e);
	}



	// 윈도우와의 상호작용
	for (auto& child : m_ChildWindows)
	{
		if (child->Interaction())
		{
			auto child_mouse_pos = child->GetMousePos();
			JGUIMouseMoveEvent child_e = e;
			child_e.pos.x = (float)child_mouse_pos.x;
			child_e.pos.y = (float)child_mouse_pos.y;
			child->JGUIMouseMove(child_e);
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
	
	if (parent == nullptr) // 최상단 루트가 되어라
	{
		if (m_ParentWindow)
		{
			if (this == m_ParentWindow->GetFocusWindow()) m_ParentWindow->SetFocusWindow(nullptr);
			m_ParentWindow->m_ChildWindows.erase(std::remove(m_ParentWindow->m_ChildWindows.begin(),
				m_ParentWindow->m_ChildWindows.end(), this), m_ParentWindow->m_ChildWindows.end());


			//// 클라이언트 창이었다가 속하게되면 위치값 ㅅ변경
			auto pos = GetTransform()->GetPosition();
			auto parent_pos = m_ParentWindow->GetTransform()->GetPosition();
			pos = pos + parent_pos;
			GetTransform()->SetLocalPosition(pos);
		}
		m_ParentWindow = nullptr;
		
		if (m_Screen == nullptr)
		{
			NewLoad();
		}
		m_WinTexture->UnBind();
		m_Priority = JGUI_WindowPriority_None;
	}
	else
	{
		if (m_Screen)
		{
			JGUI::RequestDestroyScreen(m_Screen->GetHandle());
			m_Screen = nullptr;
		}
		if (m_ParentWindow)
		{
			if (this == m_ParentWindow->GetFocusWindow()) m_ParentWindow->SetFocusWindow(nullptr);
			m_ParentWindow->m_ChildWindows.erase(std::remove(m_ParentWindow->m_ChildWindows.begin(),
				m_ParentWindow->m_ChildWindows.end(), this), m_ParentWindow->m_ChildWindows.end());
		}

		m_Priority = parent->ChildWindowSortByPriority();
		parent->m_ChildWindows.push_back(this);
		m_ParentWindow = parent;


		//// 클라이언트 창이었다가 속하게되면 위치값 ㅅ변경
		if (m_ParentWindow->GetParent() == nullptr)
		{
			auto pos = GetTransform()->GetPosition();
			auto parent_pos = parent->GetTransform()->GetPosition();
			pos = pos - parent_pos;
			GetTransform()->SetLocalPosition(pos);
		}

		m_WinTexture->Bind(m_GUIModule->GetTextureCacheKey());
	}

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

std::vector<JGUIComponent*>& JGUIWindow::GetWindowComponents()
{
	return m_WindowComponents;
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
void JGUIWindow::DestroyJGUIComponent(JGUIComponent* com)
{
	JGUI::DestroyObject(com);
	if (com == GetFocusComponent()) SetFocusComponent(nullptr);
}
JVector2 JGUIWindow::ConvertToScreenPos(const JVector2& pos)
{
	JVector2 result;
	auto size = GetTransform()->GetSize();
	result.x = pos.x - (size.x * 0.5f);
	result.y = -pos.y + (size.y * 0.5f);

	return result;
}
JGUIRect JGUIWindow::ConvertToScreenRect(const JGUIRect& rect)
{
	JGUIRect result;
	auto size = GetTransform()->GetSize();


	JVector2 lt = { result.left , result.top };
	lt = ConvertToScreenPos(lt);

	JVector2 rb = { result.right, result.bottom };
	rb = ConvertToScreenPos(rb);

	result.left = lt.x;
	result.right = rb.x;
	result.top = lt.y;
	result.bottom = rb.y;

	return result;


}
JVector2 JGUIWindow::ConvertToWorldPos(const JVector2& pos)
{
	JVector2 result;
	auto size = GetTransform()->GetSize();
	result.x = pos.x  + (size.x * 0.5f);
	result.y = -pos.y + (size.y * 0.5f);

	return result;
}
JGUIRect JGUIWindow::ConvertToWorldRect(const JGUIRect& pos)
{
	JGUIRect result;
	auto size = GetTransform()->GetSize();


	JVector2 lt = { result.left , result.top };
	lt = ConvertToWorldPos(lt);

	JVector2 rb = { result.right, result.bottom };
	rb = ConvertToWorldPos(rb);

	result.left = lt.x;
	result.right = rb.x;
	result.top = lt.y;
	result.bottom = rb.y;

	return result;
}
JVector2Int JGUIWindow::GetMousePos()
{
	if (GetParent() == nullptr) return JGUI::GetMousePos(GetRootWindowHandle());
	auto pos = GetParent()->GetMousePos();
	auto window_pos = GetTransform()->GetPosition();
	pos.x -= (int)window_pos.x;
	pos.y -= (int)window_pos.y;

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

void JGUIWindow::SetFocusComponent(JGUIComponent* com)
{
	if (m_FocusComponent != com)
	{
		if (m_FocusComponent) // exit
		{
			JGUIFocusExitEvent exit_e;
			exit_e.nextFocus = com;
			m_FocusComponent->JGUIFocusExit(exit_e);
		}
		JGUIFocusEnterEvent enter_e;
		enter_e.prevFocus = m_FocusComponent;
		//
		m_FocusComponent = com;
		if (com) // enter
		{
			com->JGUIFocusEnter(enter_e);
		}
	}
}
JGUIComponent* JGUIWindow::GetFocusComponent()
{
	return m_FocusComponent;
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
	ProcessByWindowFlags(flag);
}
EJGUI_WindowFlags JGUIWindow::GetWindowFlags() const
{
	return m_Flags;
}
void JGUIWindow::SetActive(bool active, bool is_hierarchy)
{
	JGUIObject::SetActive(active, is_hierarchy);
	for (auto& com : m_WindowComponents)
	{
		com->SetActive(active, is_hierarchy);
	}
	for (auto& win : m_ChildWindows)
	{
		win->SetActive(IsActive(), true);
	}
}
void JGUIWindow::ProcessByWindowFlags(EJGUI_WindowFlags flag)
{



}
void JGUIWindow::Init(const std::string& name, EJGUI_WindowFlags flag)
{
	m_Flags = flag;
	SetName(name);


	// 1. RenderItemManager에 ID 등록
	RE::RenderEngine::RegisterRIManager(GetID());

	// 2. Transform 생성
	RegisterTransform(this, JGUI_RectTransform_Window); 	
	RegisterCollider(this, JGUI_Collider_Box);
	GetTransform()->Flush();
	//
	auto transform = GetTransform();
	auto window_size = transform->GetSize();


	// 3. Panel 생성
	m_Panel = JGUI::CreateJGUIComponent<JGUIPanel>(GetName() + "Panel", this,  JGUI_ComponentFlag_None);
	m_Panel->RegisterCollider(this, JGUI_Collider_Box);
	m_WindowComponents.push_back(m_Panel);


	m_Panel->GetTransform()->SetSize(GetTransform()->GetSize());
	m_Panel->SetColor({ 1.0f,0.0f,1.0f,0.3f });

	// 4. 윈도우 텍스쳐 생성
	m_WinTexture = CreateJGUIComponent<JGUIWindowTexture>("WindowTexture");
	GetTransform()->AttachTransform(m_WinTexture->GetTransform());

	// 5. GUI 모듈 준비
	ReadyGUIModule();


	// 6. FLAG 에 따른 윈도우 제작
	if (m_Flags & JGUI_WindowFlag_NewLoad)
	{
		SetParent(nullptr);
	}


}

void JGUIWindow::NewLoad()
{
	if (m_Screen)
	{
		JGUI::RequestDestroyScreen(m_Screen->GetHandle());
		m_Screen = nullptr;
	}
	m_Screen = JGUI::ReqeustRegisterJGUIScreen(this);

	if (m_Screen)
	{
		m_Screen->BindGUIModuleClone(m_GUIModule.get());
	}
}

void JGUIWindow::ReadyGUIModule()
{
	auto size = GetTransform()->GetSize();
	m_ReCamera = make_shared<RE::ReCamera>();
	m_ReCamera->SetLens(45, size.x, size.y);
	m_ReCamera->SetPosition({ 0.0f,0.0f,-10.0f });
	m_ReCamera->ConvertOrthographic();
	m_GUIModule = RE::RenderEngine::GetGUIModule()->Clone(m_ReCamera.get(), GetID());
	//
	RE::RenderEngine::UnRegisterTexture(m_GUIModule->GetTextureCacheKey());
	RE::RenderEngine::RegisterTexture(m_GUIModule->GetTextureCacheKey(), m_GUIModule->GetRTTexture(0));
	//
	if (GetParent())
	{
		m_WinTexture->Bind(m_GUIModule->GetTextureCacheKey());
	}

}
bool JGUIWindow::Interaction()
{
	if (GetParent() == nullptr) return true;


	auto pos = GetParent()->GetMousePos();


	return IsActive() && GetCollider() && GetCollider()->CheckInPoint(pos);
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


