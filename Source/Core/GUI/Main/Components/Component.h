#pragma once


#include "GUIObject.h"


namespace GUI
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
	protected:
		virtual void Awake()   {}
		virtual void Start()   {}
		virtual void Tick()    {}
		virtual void Destroy() {}

		
	private:
		Element* m_Owner          = nullptr;
		bool     m_IsActive       = false;
		bool     m_IsExecuteStart = false;
	};
}