#pragma once
#include "JGUIObject.h"
#include <string>

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
protected:
	virtual void Resize(const JGUIResizeEvent& e) {}
	virtual void MouseBtDown(const JGUIKeyDownEvent& e) {}
	virtual void MouseBtUp(const JGUIKeyUpEvent& e) {}
	virtual void MouseMove(const JGUIMouseMoveEvent& e) {}
	virtual void MouseLeave() {}
	virtual void MouseHover() {}
	//virtual void 
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
public:
	template<typename ComponentType>
	ComponentType* CreateJGUIComponent(const std::string& name)
	{
		auto com = JGUI::CreateJGUIComponent<ComponentType>(name, m_OwnerWindow);
		com->SetParent(this); // <- 여기서 자식배열에 추가
		return com;
	}
	void RegisterBoxCollider();
private:
	void Init(const std::string& name, JGUIWindow* owner_window);
private:
	JGUIWindow* m_OwnerWindow = nullptr;

	// 부모
	JGUIComponent* m_Parent   = nullptr;

	// 자식 컴포넌트
	std::vector<JGUIComponent*> m_ChildComponents;
	JGUIRectTransform* m_RectTransform = nullptr;
	JGUICollider*      m_Collider = nullptr;
	JVector2 m_PrevSize;
	bool m_IsMouseTracking = false;
};