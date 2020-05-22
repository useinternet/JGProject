#pragma once

#include "GUI/JGUI.h"
#include "GUI/JGUIComponent/JGUIComponent.h"
#include "GUI/JGUIComponent/JGUIElementComponent.h"
#include "GUI/JGUIComponent/JGUIWindowComponent.h"
#include "GUI/JGUIComponent/JGUITransform.h"
#include "GUI/JGUIComponent/JGUICollider.h"
#include <functional>

class JGUIWindow;
class JGUIElement;
class JGUITransform;
class JGUICollider;
class JGUIComponent;
class JGUIElementComponent;
class JGUIWindowComponent;
class JGENGINE_API JGUIObject
{
	friend JGUI;
	friend JGUIWindow;
	friend JGUIElement;
public:
	JGUIObject( const std::string& name = "JGUIObject");
	
protected:
	virtual void Awake() {}
	virtual void Start() {}
	virtual void Destroy() {}
	virtual void Tick(const JGUITickEvent& e) {}


	float GetTick() const;
private:
	virtual void JGUIAwake();
	virtual void JGUIStart() { Start(); m_Is_ExecuteStart = true; }
	virtual void JGUIDestroy();
	virtual void JGUITick(const JGUITickEvent& e);
public:
	const std::string& GetName() const {
		return m_Name;
	}
	void               SetName(const std::string& name) {
		m_Name = name;
	}
	virtual void SetActive(bool active, bool is_hierarchy = false);
	bool IsActive() const {
		return m_Active && m_ActiveSelf;
	}
	uint64_t GetID() const {
		return m_ID;
	}

	JGUITransform* GetTransform() {
		return m_Transform;
	}
	JGUICollider* GetCollider() {
		return m_Collider;
	}
	bool IsDestroying() const {
		return m_IsDestroying;
	}
	bool IsWindow() const {
		return m_IsWindow;
	}
public:
	template<typename ComponentType>
	ComponentType* CreateJGUIComponent()
	{
		ComponentType* result = JGUI::CreateJGUIComponent<ComponentType>(typeid(ComponentType).name());
		m_Coms.push_back(result);
		if (m_IsWindow) result->SetOwner((JGUIWindow*)this);
		else result->SetOwner((JGUIElement*)this);
		result->JGUIAwake();
		return result;
	}
	void DestroyComponent(JGUIComponent* com);

	template<typename ComponentType>
	ComponentType* FindComponent() 
	{
		for (auto& com : m_Coms)
		{
			if (typeid(*com) == typeid(ComponentType))
				return (ComponentType*)(com);
		}
		return nullptr;
	}
	JGUIComponent* FindComponent(const std::string& name) const;
	JGUIComponent* GetComponent(int n) const;
	uint32_t       GetComponentCount() const;
	void RegisterCollider(EJGUI_Collider type);
private:
	bool IsExecuteStartFunc() const {
		return m_Is_ExecuteStart;
	}
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
	JGUITransform*     m_Transform = nullptr;
	JGUICollider*      m_Collider  = nullptr;


	std::vector<JGUIComponent*> m_Coms;
	std::queue<JGUIComponent*>  m_DestroyingComponent;
	bool m_IsWindow = false;
};