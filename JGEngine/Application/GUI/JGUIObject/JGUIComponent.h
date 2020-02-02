#pragma once
#include "JGUIObject.h"
#include <string>
#include <set>
class RenderItem;
class JGUIWindow;
class JGUICollider;
class JGUIRectTransform;


class JGUIComponent : public JGUIObject
{
	friend JGUI;
	friend JGUIWindow;
public:
	JGUIComponent(const std::string& name = "JGUIComponent") : JGUIObject(name) {}
	JGUIWindow* GetOwnerWindow() const;
protected:
	virtual void JGUIAwake() override;
	virtual void JGUIStart() override;
	virtual void JGUITick(const JGUITickEvent& e) override;
	virtual void JGUIDestroy() override;


	virtual void JGUIResize(const JGUIResizeEvent& e);
	virtual void JGUIMouseMove(const JGUIMouseMoveEvent& e);
	virtual void JGUIMouseBtDown(const JGUIKeyDownEvent& e);
	virtual void JGUIMouseBtUp(const JGUIKeyUpEvent& e);
	virtual void JGUIMouseLeave();
	virtual void JGUIMouseHover();

	virtual void JGUIParentUpdateNotification();
protected:
	virtual void Resize(const JGUIResizeEvent& e) {}
	virtual void MouseBtDown(const JGUIKeyDownEvent& e) {}
	virtual void MouseBtUp(const JGUIKeyUpEvent& e) {}
	virtual void MouseMove(const JGUIMouseMoveEvent& e) {}
	virtual void MouseLeave() {}
	virtual void MouseHover() {}
	virtual void ParentUpdateNotification() {}
public:
	virtual void SetParent(JGUIComponent* parent);
	JGUIComponent* GetParent() const;
	const std::vector<JGUIComponent*>& GetChilds() const;
	std::vector<JGUIComponent*>& GetChilds();
	JGUIComponent* FindChild(uint32_t index);
	JGUIComponent* FindChild(const std::string& name);


	JGUIRectTransform* GetTransform() const {
		return m_RectTransform;
	}
	JGUICollider* GetCollider() const {
		return m_Collider;
	}
	//bool IsEventLock(EJGUI_ComponentEvents event_type);
	//void EventLock(EJGUI_ComponentEvents event_type, EJGUI_ComponentEventLockFlags flag = JGUI_ComponentEventLockFlag_None);
	//void EventUnLock(EJGUI_ComponentEvents event_type, EJGUI_ComponentEventLockFlags flag = JGUI_ComponentEventLockFlag_None);

public:
	template<typename ComponentType>
	ComponentType* CreateJGUIComponent(const std::string& name)
	{
		auto com = JGUI::CreateJGUIComponent<ComponentType>(name, m_OwnerWindow);
		com->SetParent(this); // <- 여기서 자식배열에 추가
		return com;
	}
	void RegisterCollider(EJGUI_Component_Colider colider_type, EJGUI_ComponentInteractionFlags flags = JGUI_ComponentInteractionFlag_Default);
protected:
	void ChildLock() { m_IsChildLock = true; }
	EJGUI_ComponentInteractionFlags GetInteractionFlag() const { return m_InteractionFlag; }
	void SetInteractionFlag(EJGUI_ComponentInteractionFlags flags) { m_InteractionFlag = flags; }

private:
	void EventProcess(EJGUI_ComponentEvents event_type, const void* data);
	void Init(const std::string& name, JGUIWindow* owner_window);
	bool Interation(EJGUI_ComponentEvents event_type);
	JGUIComponent* TrackingCanInteractionComponent();
private:
	JGUIWindow* m_OwnerWindow = nullptr;

	// 부모
	JGUIComponent* m_Parent   = nullptr;
	std::map<EJGUI_ComponentEvents, bool> m_EventLockMap;
	std::set<EJGUI_ComponentEvents>       m_OnceEventLockMap;


	// 플래그
	EJGUI_ComponentInteractionFlags m_InteractionFlag = JGUI_ComponentInteractionFlag_None;


	// 자식 컴포넌트
	std::vector<JGUIComponent*> m_ChildComponents;
	JGUIRectTransform* m_RectTransform = nullptr;
	JGUICollider*      m_Collider = nullptr;
	JVector2 m_PrevSize;
	bool m_IsMouseTracking = false;
	bool m_IsChildLock = false;
};