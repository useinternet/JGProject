#include "pch.h"
#include "IGWindow.h"
#include "IGManager.h"
#include "GUI/EditorGUI.h"


namespace JE
{
	IGWindow::IGWindow() :
		m_Name("None"), m_IsOpen(true), m_pManager(nullptr), m_IsFocused(false),
		m_WindowFlags(ImGuiWindowFlags_None) { }

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
				m_IsFocused = ImGui::IsWindowFocused();
				OnGUI();
				ImGui::End();
			}
		}
	}
	void IGWindow::Destroy()
	{
		OnDestroy();
	}
	void IGWindow::ReceiveEvent(Event& e)
	{
		if (m_IsOpen)
		{
			OnEvent(e);

			// RenderEvent 
			if (e.GetEventType() == EventType::RenderEvent)
			{
				RenderEvent render_e = CONVERT_EVENT(RenderEvent, e);
				// RenderDevice Event
				if (render_e.GetRenderEventType() == RenderEventType::RenderDevice)
				{
					RenderDeviceEvent rd_e = CONVERT_EVENT(RenderDeviceEvent, e);
					switch (rd_e.GetRenderDeviceEventType())
					{
					case RenderDeviceEventType::SendManagedResource:
						SendManagedResource(CONVERT_EVENT(SendManagedResourceEvent, e));
						return;
					case RenderDeviceEventType::GUIAllocatorReAllocated:
						GUIAllocatorReAllocated(CONVERT_EVENT(GUIAllocatorReAllocatedEvent, e));
						return;
					}

				}

			}

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

	bool IGWindow::IsFocused() const
	{
		return m_IsFocused;
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


