#pragma once
#include "GUIObject.h"
#include "Components/Transform.h"
#include "ElementTemplates.h"
namespace JGUI
{
	class Component;
	class Window;
	class LayerComponent;
	class DrawComponent;
	class NativeScriptComponent;

#define ELEMENT_EVENT_IMP(EventName)
#define ELEMENT_EVENT_IMP_ARG1(EventName, arg1)
#define ELEMENT_EVENT_IMP_ARG2(EventName, arg1, arg2)



	class GUI_API Element : public GUIObject
	{
		friend Transform;
		friend LayerComponent;
		friend DrawComponent;
		friend Window;
		friend GUIIF;
	public:
		virtual ~Element() {}
	private:
		void GUIAwake();
		void GUIStart();
		void GUITick();
		void GUIDestroy();
	protected:
		virtual void Awake()   {}
		virtual void Start()   {}
		virtual void Tick()    {}
		virtual void Destroy() {}
	protected:
		virtual void OnMouseButtonDown(KeyCode bt);
		virtual void OnMouseButton(KeyCode bt);
		virtual void OnMouseButtonUp(KeyCode bt);
													
		virtual void OnMouseClick(KeyCode bt);
		virtual void OnMouseDoubleClick(KeyCode bt);


		virtual void OnMouseEnter();
		virtual void OnMouseMove();
		virtual void OnMouseExit();


		virtual void OnKeyDown(KeyCode code);
		virtual void OnKey(KeyCode code);
		virtual void OnKeyUp(KeyCode code);


		virtual void OnResize(float width, float height);
		virtual void OnFocusEnter();
		virtual void OnFocus();
		virtual void OnFocusExit();

		virtual void OnDropItem(const GUIDropItem& item);
		virtual void OnChar(wchar_t);
	public:
		template<typename T>
		T* CreateElement(const std::wstring& name, ElementFlags flags = ElementFlag_None) {
			T* element = GUIIF::AllocateGUIObject<T>();
			element->SetName(name);
			element->m_Flags = flags;
			element->m_OwnerWindow = m_OwnerWindow;
			element->GUIAwake();
			element->GetTransform()->SetParent(m_Transform);
			
			return element;
		}


		template<typename T>
		T* CreateComponent(const std::wstring& name = s2ws(typeid(T).name())) {
			T* result = GUIIF::AllocateGUIObject<T>();
			result->SetName(name);
			result->m_Owner = this;
			m_Components.push_back(result);
			((Component*)result)->Awake();

			if (std::is_base_of<NativeScriptComponent, T>::value)
			{
				m_ScriptComponents[(Component*)result] = (NativeScriptComponent*)result;
			}
			return result;
		}



		void DestroyComponent(Component* component);
		void DestroyElement(Element* element);



		Component* FindComponent(const std::wstring& name);
		Component* FindComponent(uint32_t index);
		template<typename T>
		T* FindComponent()
		{
			for (auto& com : m_Components)
			{
				if (typeid(*com) == typeid(T))
					return (T*)(com);
			}
			return nullptr;
		}

		Window* GetOwnerWindow() const {
			return m_OwnerWindow;
		}
		Transform* GetTransform() const {
			return m_Transform;
		}
		ElementFlags GetFlags() const {
			return m_Flags;
		}

		bool IsActive() const {
			return m_IsActiveSelf && m_IsActive;
		}
		void SetActive(bool is_active);
	private:
		void SetActiveHierarchy(bool is_active);
	private:
		std::vector<Component*> m_Components;
		std::unordered_map<Component*, NativeScriptComponent*> m_ScriptComponents;

		// 
		LayerComponent* m_LayerComponent = nullptr;
		Transform*      m_Transform      = nullptr;
		ElementFlags    m_Flags = ElementFlag_None;
		Window*         m_OwnerWindow    = nullptr;

		// Setting
		bool       m_IsExecuteStart = false;
		JVector2   m_PrevSize = { -1,-1 };




		bool m_IsActiveSelf = true;
		bool m_IsActive     = true;
	};





}