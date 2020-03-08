#pragma once

#include "GUI/JGUI.h"
#include <functional>

class JGUIWindow;
class JGUIComponent;
class JGUIRectTransform;
class JGUIShape;
class JGUICollider;
class JGUIObject
{
	friend JGUI;
	friend JGUIWindow;
	friend JGUIComponent;
public:
	JGUIObject(const std::string& name = "JGUIObject") :
		m_Name(name) {}
protected:
	virtual void Awake() {}
	virtual void Start() {}
	virtual void Destroy() {}
	virtual void Tick(const JGUITickEvent& e) {}


	float GetTick() const;
private:
	virtual void JGUIAwake(){ Awake(); };
	virtual void JGUIStart() { Start(); m_Is_ExecuteStart = true; }
	virtual void JGUIDestroy();
	virtual void JGUITick(const JGUITickEvent& e) { Tick(e); }
public:
	const std::string& GetName() const {
		return m_Name;
	}
	void               SetName(const std::string& name) {
		m_Name = name;
	}
	virtual void SetActive(bool active, bool is_hierarchy = false) {
		m_Active     = active;
		if (!is_hierarchy)
		{
			m_ActiveSelf = active;
		}
	}
	bool IsActive() const {
		return m_Active && m_ActiveSelf;
	}
	bool IsExecuteStartFunc() const {
		return m_Is_ExecuteStart;
	}
	uint64_t GetID() const {
		return m_ID;
	}

	JGUIRectTransform* GetTransform() {
		return m_Transform;
	}
	JGUICollider* GetCollider() {
		return m_Collider;
	}
	bool IsDestroying() const {
		return m_IsDestroying;
	}


	
public:
	void RegisterCollider(JGUIWindow* owner_window, EJGUI_Colider type);
	void RegisterTransform(JGUIComponent* owner_com, EJGUI_RectTransform type);
	void RegisterTransform(JGUIWindow* owner_window,    EJGUI_RectTransform type);
private:
	std::string m_Name;
	std::string m_LayerName = "None";
	//
	bool        m_Active = true;
	bool        m_ActiveSelf = true;
	//
	uint64_t    m_ID = -1;
	//
	bool m_Is_ExecuteStart = false;
	bool m_IsDestroying = false;
	// Flags

	// 기본 컴포넌트들
	JGUIRectTransform* m_Transform = nullptr;
	JGUICollider*      m_Collider  = nullptr;
};