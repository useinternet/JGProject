#pragma once
#include "GUI/EditorGUI/ImGui/imgui.h"
#include "CommonCore.h"
#include <vector>
#include <string>
namespace JE
{
	class IGManager;
	class IGWindow
	{

	private:
		bool m_IsOpen;
		bool m_IsFocused;
		std::string m_Name;
		ImGuiWindowFlags m_WindowFlags;
		IGManager* m_pManager;
	public:
		IGWindow();
		void Load(IGManager* manager);
		void Update();
		void Destroy();
	    void ReceiveEvent(Event& e);
	public:
		const std::string& GetName() const;
		const void SetName(const std::string& name);
		void SetWindowFlags(ImGuiWindowFlags flags);
		void AddWindowFlags(ImGuiWindowFlags flags);
		void Close();
		bool IsFocused() const;
		bool IsOpen() const;
	protected: // RenderEvent
		virtual void OnEvent(Event& e) {}
		virtual void SendManagedResource(SendManagedResourceEvent& e) {}
		virtual void GUIAllocatorReAllocated(GUIAllocatorReAllocatedEvent& e) {}
	protected:
		virtual void OnLoad() = 0;
		virtual void OnGUI() = 0;

		virtual void OnDestroy() = 0;
		void EventNotify(Event& e);
		IGManager* GetIGManager();
	};

}