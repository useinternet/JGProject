#pragma once
#include "GUI/EditorGUI/ImGui/imgui.h"
#include "CommonCore.h"
#include <vector>
#include <string>
class IGWindowEventListener;
namespace JE
{
	class IGManager;
	class IGWindow
	{
		friend IGWindowEventListener;
	private:
		bool m_IsOpen;
		std::string m_Name;
		ImGuiWindowFlags m_WindowFlags;
		std::shared_ptr<EventListener> m_EventListener;
		IGManager* m_pManager;
	public:
		IGWindow();
		void Load(IGManager* manager);
		void Update();
		void Destroy();
	    void OnEvent(Event& e);
	public:
		const std::string& GetName() const;
		const void SetName(const std::string& name);
		void SetWindowFlags(ImGuiWindowFlags flags);
		void AddWindowFlags(ImGuiWindowFlags flags);
		void Close();
		bool IsOpen() const;


		virtual void DataLoad(std::ifstream& fin);
		virtual void DataSave(std::ofstream& fout);
	protected: // RenderEvent
		virtual void OnEventFromRE(ToEditFromReEvent& e) {}
	protected:
		virtual void OnLoad() = 0;
		virtual void OnGUI() = 0;
		virtual void OnDestroy() = 0;
		void EventNotify(Event& e);
		IGManager* GetIGManager();

		template<typename Listener>
		void RegisterEventListener(const Listener& listener) {
			if (m_EventListener)
				m_EventListener.reset();
			m_EventListener = std::make_shared<Listener>(listener);
		}
	};

}


class IGWindowEventListener : public EventListener
{
public:
	JE::IGWindow* Owner = nullptr;
	IGWindowEventListener(JE::IGWindow* owner);
protected:
	virtual void OnRenderEvent(RenderEvent& e) override;
	virtual void OnAppEvent(AppEvent& e) override;
	virtual void OnInputEvent(InputEvent& e) override;
	virtual void OnPhysicsEvent(PhysicsEvent& e) override;
	virtual void OnSoundEvent(SoundEvent& e) override;
	virtual void OnGmaeFrameWorkEvent(GameFrameWorkEvent& e) override;
	virtual void OnWindowEvent(WindowEvent& e) override;
};

