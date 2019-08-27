#include "pch.h"
#include "IGWindow.h"
#include "IGManager.h"
#include "GUI/EditorGUI.h"




namespace JE
{
	IGWindow::IGWindow() :
		m_Name("None"), m_IsOpen(true), m_pManager(nullptr),
		m_WindowFlags(ImGuiWindowFlags_None)
	{
		RegisterEventListener(IGWindowEventListener(this));
	}

	void IGWindow::Load(IGManager* manager)
	{
		m_pManager = manager;
		OnLoad();
	}
	void IGWindow::Update()
	{
		if (m_IsOpen)
		{
			if (!ImGui::Begin(m_Name.c_str(), &m_IsOpen, m_WindowFlags))
			{
				ImGui::End();
			}
			else
			{
				OnGUI();
				ImGui::End();
			}
		}
	}
	void IGWindow::Destroy()
	{
		OnDestroy();
	}
	void IGWindow::OnEvent(Event& e)
	{
		if (m_IsOpen && m_EventListener)
		{
			m_EventListener->OnEvent(e);
		}
	}
	const std::string& IGWindow::GetName() const
	{
		return m_Name;
	}
	const void IGWindow::SetName(const std::string& name)
	{
		m_Name = name;
	}
	void IGWindow::SetWindowFlags(ImGuiWindowFlags flags)
	{
		m_WindowFlags = flags;
	}
	void IGWindow::AddWindowFlags(ImGuiWindowFlags flags)
	{
		m_WindowFlags |= flags;
	}
	void IGWindow::Close()
	{
		m_IsOpen = false;
	}
	bool IGWindow::IsOpen() const
	{
		return m_IsOpen;
	}
	void IGWindow::EventNotify(Event& e)
	{
		GlobalLinkData::OnEvent(e);
	}
	IGManager* IGWindow::GetIGManager()
	{
		return m_pManager;
	}
	
}

IGWindowEventListener::IGWindowEventListener(JE::IGWindow* owner) : Owner(owner) {}

void IGWindowEventListener::OnRenderEvent(RenderEvent& e)
{
	static std::mutex RenderMutex;
	std::lock_guard<std::mutex> lock(RenderMutex);
	if (Owner == nullptr)
		return;
	if (e.GetRenderEventType() == RenderEventType::ToEditFromRe)
	{
		if (CONVERT_EVENT(ToEditFromReEvent, e).SendIGWindowID != (uint64_t)Owner)
			return;

		Owner->OnEventFromRE(CONVERT_EVENT(ToEditFromReEvent, e));
	}
}
void IGWindowEventListener::OnAppEvent(AppEvent& e)
{
	if (Owner == nullptr)
		return;
}
void IGWindowEventListener::OnInputEvent(InputEvent& e)
{
	if (Owner == nullptr)
		return;
}
void IGWindowEventListener::OnPhysicsEvent(PhysicsEvent& e)
{
	if (Owner == nullptr)
		return;
}
void IGWindowEventListener::OnSoundEvent(SoundEvent& e)
{
	if (Owner == nullptr)
		return;
}
void IGWindowEventListener::OnGmaeFrameWorkEvent(GameFrameWorkEvent& e)
{
	if (Owner == nullptr)
		return;
}
void IGWindowEventListener::OnWindowEvent(WindowEvent& e)
{
	if (Owner == nullptr)
		return;
}