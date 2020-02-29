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
private:
	static uint64_t m_RIOffset;
	static void RISortingOrderReset() {
		m_RIOffset = 0;
	}
public:
	JGUIComponent(const std::string& name = "JGUIComponent") : JGUIObject(name) {}
	JGUIWindow* GetOwnerWindow() const;
	virtual void SetActive(bool active, bool is_hierarchy = false) override;
protected:
	virtual void JGUIAwake() override;
	virtual void JGUIStart() override;
	virtual void JGUITick(const JGUITickEvent& e) override;
	virtual void JGUIDestroy() override;


	virtual void JGUIResize(const JGUIResizeEvent& e);
	virtual void JGUIChar(const JGUICharEvent& e);
	virtual void JGUIKeyDown(const JGUIKeyDownEvent& e);
	virtual void JGUIKeyUp(const JGUIKeyUpEvent& e);
	virtual void JGUIMouseMove(const JGUIMouseMoveEvent& e);
	virtual void JGUIMouseBtDown(const JGUIKeyDownEvent& e);
	virtual void JGUIMouseBtUp(const JGUIKeyUpEvent& e);
	virtual void JGUIMouseLeave();
	virtual void JGUIMouseHover();

	virtual void JGUIFocusEnter(const JGUIFocusEnterEvent& e);
	virtual void JGUIFocusExit(const JGUIFocusExitEvent& e);
	virtual void JGUIOnFocus();
protected:
	virtual void Resize(const JGUIResizeEvent& e) {}
	virtual void Char(const JGUICharEvent& e) {}
	virtual void KeyDown(const JGUIKeyDownEvent& e) {}
	virtual void KeyUp(const JGUIKeyUpEvent& e) {}
	virtual void MouseBtDown(const JGUIKeyDownEvent& e) {}
	virtual void MouseBtUp(const JGUIKeyUpEvent& e) {}
	virtual void MouseMove(const JGUIMouseMoveEvent& e) {}
	virtual void MouseLeave() {}
	virtual void MouseHover() {}
	virtual void FocusEnter(const JGUIFocusEnterEvent& e) {}
	virtual void FocusExit(const JGUIFocusExitEvent& e) {}
	virtual void OnFocus() {}
public:
	virtual void SetParent(JGUIComponent* parent);
	JGUIComponent* GetParent() const;

	std::vector<JGUIComponent*> GetChilds() const;


	EJGUI_ComponentFlags GetFlags() const {
		return m_Flags;
	}
	uint32_t GetPriority() const {
		return m_Priority;
	}
	void SetPriority(uint32_t priority);
	uint64_t GetRISortingOrder() const {
		return m_RISortingOrder;
	}
	void SetLayer(uint64_t layer);
	uint64_t GetLayer() const {
		return m_Layer;
	}
public:
	template<typename ComponentType>
	ComponentType* CreateJGUIComponent(const std::string& name, EJGUI_ComponentFlags flag = JGUI_ComponentFlag_None)
	{
		auto com = JGUI::CreateJGUIComponent<ComponentType>(name, m_OwnerWindow, flag);
		if ((m_Flags & JGUI_ComponentFlag_NoChild) == false) com->SetParent(this);
		return com;
	}
	void DestroyJGUIComponent(JGUIComponent* com);
private:
	void Init(const std::string& name, JGUIWindow* owner_window);
	bool Interation();
	JGUIComponent* TrackingCanInteractionComponent();
private:
	JGUIWindow* m_OwnerWindow = nullptr;

	// 부모
	JGUIComponent* m_Parent   = nullptr;


	// 플래그
	EJGUI_ComponentFlags m_Flags;

	std::map<uint32_t, std::vector<JGUIComponent*>> m_ChildComponents;

	JVector2 m_PrevSize;
	bool     m_IsMouseTracking = false;
	uint32_t m_Priority       = 0;
	uint64_t m_RISortingOrder = 0;
	uint64_t m_Layer = 0;
};