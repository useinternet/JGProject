#pragma once
#include "GUI/JGUI.h"



class JGUIElement;
class JGUIWindow;
class JGUIObject;
class JGUIElementComponent;
class JGUIWindowComponent;
class JGUITransform;
class JGENGINE_API JGUIComponent
{
	friend JGUIObject;
	friend JGUIWindow;
	friend JGUIElement;
	friend JGUIElementComponent;
	friend JGUIWindowComponent;
	friend JGUI;
private:
	virtual void JGUIAwake() { Awake(); };
	virtual void JGUIStart() { 
		Start();
		m_IsExecuteStart = true;
	};
	virtual void JGUIDestroy() { Destroy(); }
	virtual void JGUITick(float tick) { Tick(tick); }
	
	virtual void JGUIChangeParent(JGUIObject* prevParent) { ChangeParent(prevParent); }
protected:
	virtual void Awake() {}
	virtual void Start() {}
	virtual void Destroy() {}
	virtual void Tick(float tick) {}
	virtual void ChangeParent(JGUIObject* prevParent) {}




	float GetTick() const;
public:
	virtual void SetActive(bool active) {
		m_IsActive = active;
	}
	bool IsActive() const {
		return m_IsActive;
	}

	const std::string& GetName() const {
		return m_Name;
	}
	void SetName(const std::string& name) {
		m_Name = name;
	}


	// private »ý°¢Áß
	void SetOwner(JGUIElement* element);
	void SetOwner(JGUIWindow* win);

	JGUITransform* GetTransform() const;

	

private:
	bool IsExecuteStart() const {
		return m_IsExecuteStart;
	}
	bool IsDestroying() const {
		return m_IsDestroying;
	}
private:
	bool         m_IsActive = true;
	std::string  m_Name = "JGUIComponent";
	bool         m_IsExecuteStart = false;
	bool         m_IsDestroying   = false;
protected:
	JGUIElement* m_ElementOwner = nullptr;
	JGUIWindow*  m_WindowOwner = nullptr;

};

