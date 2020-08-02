#pragma once
#include "GUIObject.h"
#include "Components/Transform.h"

namespace GUI
{
	class Window;
	class LayerComponent;
	class DrawComponent;
	class GUI_API Element : public GUIObject
	{
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
		virtual void OnMouseButtonDown(KeyCode bt) {}
		virtual void OnMouseButton(KeyCode bt) {}
		virtual void OnMouseButtonUp(KeyCode bt) {}

		virtual void OnMouseClick(KeyCode bt) {}
		virtual void OnMouseDoubleClick(KeyCode bt) {}


		virtual void OnMouseEnter() {}
		virtual void OnMouseMove()  {}
		virtual void OnMouseExit()  {}


		virtual void OnKeyDown(KeyCode code) {}
		virtual void OnKey(KeyCode code) {}
		virtual void OnKeyUp(KeyCode code) {}


		virtual void OnResize(float width, float height) {}
		virtual void OnFocusEnter() {}
		virtual void OnFocus() {}
		virtual void OnFocusExit() {}
	public:
		template<typename T>
		T* CreateElement(const std::wstring& name, ElementFlags flags = ElementFlag_None) {
			T* element = GUIIF::AllocateGUIObject<T>();
			element->SetName(name);
			element->m_OwnerWindow = m_OwnerWindow;
			element->GUIAwake();
			element->GetTransform()->SetParent(m_Transform);
			
			return element;
		}
		template<typename T>
		T* CreateComponent(const std::wstring& name) {
			T* result = GUIIF::AllocateGUIObject<T>();
			result->SetName(name);
			result->m_Owner = this;
			m_Components.push_back(result);
			((Component*)result)->Awake();
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
	private:
		std::vector<Component*> m_Components;
		LayerComponent* m_LayerComponent = nullptr;
		Transform*      m_Transform      = nullptr;


		Window*    m_OwnerWindow    = nullptr;
		bool       m_IsExecuteStart = false;

		JVector2   m_PrevSize = { -1,-1 };
	};
}