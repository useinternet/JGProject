#include "pch.h"
#include "JGUIWindow.h"
#include "JGUIComponent.h"
#include "GUI/JGUIComponent/JGUIRectTransform.h"
#include "GUI/JGUIComponent/JGUIPanel.h"
#include "GUI/JGUIComponent/JGUICollider.h"

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
	RE::RenderEngine::UnRegisterRIManager(GetID());
	// 부모 윈도우 배열에서 삭제
	if (m_ParentWindow)
	{
		auto& childs = m_ParentWindow->m_ChildWindows;
		childs.erase(std::remove_if(childs.begin(), childs.end(), [&](JGUIWindow* win)
		{
			return win != this;
		}), childs.end());
	}
	// 자식들 손절
	for (auto& window : m_ChildWindows)
	{
		JGUI::DestroyObject(window);
	}
	// 컴포넌트 삭제
	for (auto& com : m_WindowComponents)
	{
		JGUI::DestroyObject(com);
		if (GetFocus() == com) SetFocus(nullptr);
	}
	m_GUIModule = nullptr;
	// 스크린 삭제 요청
	if (m_Screen)
	{
		JGUI::RequestDestroyScreen(m_Screen->GetHandle());
	}
}

void JGUIWindow::JGUITick(const JGUITickEvent& e)
{
	JGUIObject::JGUITick(e);
	


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
	//ENGINE_LOG_INFO(e.ToString());
	GetTransform()->SetSize(e.width, e.height);
	m_Panel->GetTransform()->SetSize(e.width, e.height);
	m_Panel->SetColor({ 1.0f,0.0f,1.0f,0.3f });
	m_ReCamera = make_shared<RE::ReCamera>();
	m_ReCamera->SetLens(45, e.width, e.height);
	m_ReCamera->SetPosition({ 0.0f,0.0f,-10.0f });
	m_ReCamera->ConvertOrthographic();
	m_GUIModule = RE::RenderEngine::GetGUIModule()->Clone(m_ReCamera.get(), GetID());


	if (m_Screen)
	{
		m_Screen->Resize((uint32_t)e.width, (uint32_t)e.height);
		m_Screen->BindGUIModuleClone(m_GUIModule.get());
	}
	Resize(e);
	// resize 구현
}
void JGUIWindow::JGUIFocusEnter(const JGUIFocusEnterEvent& e)
{
	//ENGINE_LOG_INFO(e.ToString() + " : " + GetName());

	FocusEnter(e);
}
void JGUIWindow::JGUIFocusExit(const JGUIFocusExitEvent& e)
{
	JGUI::InputFlush();
	SetFocus(nullptr);
	ENGINE_LOG_INFO(e.ToString() + " : " + GetName());
	FocusExit(e);
}
void JGUIWindow::JGUIOnFocus()
{
	OnFocus();
}

void JGUIWindow::JGUIChar(const JGUICharEvent& e)
{
	Char(e);
}

void JGUIWindow::JGUIKeyDown(const JGUIKeyDownEvent& e)
{
	//ENGINE_LOG_INFO(e.ToString());
	KeyDown(e);
}
void JGUIWindow::JGUIKeyUp(const JGUIKeyUpEvent& e)
{
	//ENGINE_LOG_INFO(e.ToString());
	KeyUp(e);
}

void JGUIWindow::JGUIMouseBtDown(const JGUIKeyDownEvent& e)
{
	JGUIComponent* cc = m_Panel;
	cc = cc->TrackingCanInteractionComponent();

	if (GetFocus() == nullptr || m_IsMouseLeave)
	{
		SetFocus(cc);
		m_IsMouseLeave = false;
	}

	if (GetFocus() && GetFocus() == cc)
	{
		cc->EventProcess(JGUI_ComponentEvent_MouseBtDown, &e);
	}
	MouseBtDown(e);
}

void JGUIWindow::JGUIMouseBtUp(const JGUIKeyUpEvent& e)
{
	auto focus = GetFocus();
	if (focus)
	{
		if (focus->Interation(JGUI_ComponentEvent_MouseBtUp))
		{
			focus->EventProcess(JGUI_ComponentEvent_MouseBtUp, &e);
		}
		SetFocus(nullptr);
	}
	MouseBtUp(e);
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
		auto p = JGUI::GetMousePos(GetRootWindowHandle());
		if (com->IsActive() && com->GetCollider() && com->GetCollider()->CheckInPoint(p))
		{
			com->JGUIMouseMove(e);
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
void JGUIWindow::Char(const JGUICharEvent& e)
{
}
void JGUIWindow::SetParent(JGUIWindow* parent)
{
	// 4. JGUIWindow용 RenderItem 비우기
	if (m_ParentWindow == nullptr && parent)
	{
		if (m_Screen)
		{
			JGUI::RequestDestroyScreen(m_Screen->GetHandle());
			m_Screen = nullptr;
		}
		parent->m_ChildWindows.push_back(this);
		m_ParentWindow = parent;
	}
	else if (parent == nullptr) // 최상단 루트가 되어라
	{
		if (m_ParentWindow)
		{
			m_ParentWindow->m_ChildWindows.erase(std::remove(m_ParentWindow->m_ChildWindows.begin(),
				m_ParentWindow->m_ChildWindows.end(), this), m_ParentWindow->m_ChildWindows.end());
		}
		m_ParentWindow = nullptr;
		if (m_Screen == nullptr)
		{
			NewLoad();
		}
	}
	else
	{
		if (m_ParentWindow)
		{
			m_ParentWindow->m_ChildWindows.erase(std::remove(m_ParentWindow->m_ChildWindows.begin(),
				m_ParentWindow->m_ChildWindows.end(), this), m_ParentWindow->m_ChildWindows.end());
		}
		parent->m_ChildWindows.push_back(this);
		m_ParentWindow = parent;
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
JVector2 JGUIWindow::ConvertToScreenPos(const JVector2& pos)
{
	JVector2 result;
	auto size = GetTransform()->GetSize();
	result.x = pos.x - (size.x / 2);
	result.y = -pos.y + (size.y / 2);

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
HWND JGUIWindow::GetRootWindowHandle() const
{
	if (m_Screen)
		return m_Screen->GetHandle();
	else if (m_ParentWindow)
		return m_ParentWindow->GetRootWindowHandle();
	else
		return 0;
}

void JGUIWindow::SetFocus(JGUIComponent* com)
{
	m_FocusComponent = com;
}
JGUIComponent* JGUIWindow::GetFocus()
{
	return m_FocusComponent;
}



void JGUIWindow::SetPriority(EJGUI_WindowPriority p)
{
	m_Priority = p;
}
void JGUIWindow::Init(const std::string& name, EJGUI_WindowFlags flag)
{
	m_Flags = flag;
	SetName(name);
	RE::RenderEngine::RegisterRIManager(GetID());
	m_RectTransform = CreateJGUIComponent<JGUIWinRectTransform>("JGUIRectTransform");
	GetTransform()->Flush();
	auto transform = GetTransform();
	auto window_size = transform->GetSize();
	m_Panel = JGUI::CreateJGUIComponent<JGUIPanel>(GetName() + "Panel", this);
	//m_Panel->RegisterCollider(JGUI_Component_Colider_Box);
	m_WindowComponents.push_back(m_Panel);
	m_Panel->GetTransform()->SetSize(GetTransform()->GetSize());
	m_Panel->SetColor({ 1.0f,0.0f,1.0f,0.3f });


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
	auto size = m_RectTransform->GetSize();
	auto pos = m_RectTransform->GetPosition();
	m_Screen = JGUI::ReqeustRegisterJGUIScreen(this);

	m_ReCamera = make_shared<RE::ReCamera>();
	m_ReCamera->SetLens(45, size.x, size.y);
	m_ReCamera->SetPosition({ 0.0f,0.0f,-10.0f });
	m_ReCamera->ConvertOrthographic();
	m_GUIModule = RE::RenderEngine::GetGUIModule()->Clone(m_ReCamera.get(), GetID());


	if (m_Screen)
	{
		m_Screen->BindGUIModuleClone(m_GUIModule.get());
	}
}
