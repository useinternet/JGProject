#pragma once


#include "GUIObject.h"


namespace JGUI
{
	class Element;
	class Window;
	class GUI_API Component : public GUIObject
	{
		friend Element;
		friend Window;
	public:
		bool IsActive() const {
			return m_IsActive;
		}
		void SetActive(bool active) {
			m_IsActive;
		}
		Element* GetOwner() const {
			return m_Owner;
		}
		Window* GetOwnerWindow() const;
		virtual ~Component() {}

		Transform* GetTransform() const;
	protected:
		virtual void Awake()   {}
		virtual void Start()   {}
		virtual void Tick()    {}
		virtual void Destroy() {}

		
	private:
		Element* m_Owner          = nullptr;
		bool     m_IsActive       = true;
		bool     m_IsExecuteStart = false;
	};
}