#pragma once

#include "GUI/JGUI.h"
#include <functional>

class JGUIWindow;
class JGUIComponent;
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
private:
	virtual void JGUIAwake(){ Awake(); };
	virtual void JGUIStart() { Start(); m_Is_ExecuteStart = true; }
	virtual void JGUIDestroy() { Destroy();};
	virtual void JGUITick(const JGUITickEvent& e) { Tick(e); }
public:
	const std::string& GetName() const {
		return m_Name;
	}
	void               SetName(const std::string& name) {
		m_Name = name;
	}
	virtual void SetActive(bool active) {
		m_Active = active;
	}
	bool IsActive() const {
		return m_Active;
	}
	bool IsExecuteStartFunc() const {
		return m_Is_ExecuteStart;
	}
	uint64_t GetID() const {
		return m_ID;
	}
private:

	std::string m_Name;
	std::string m_LayerName = "None";
	//
	bool        m_Active = true;
	//
	uint64_t    m_ID = -1;

	bool m_Is_ExecuteStart = false;
};