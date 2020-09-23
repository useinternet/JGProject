#pragma once

#include "Component.h"
#include "Elements/Element.h"
#include "Transform.h"

namespace JGUI
{
	class GUI_API NativeScriptComponent : public Component
	{
		friend class Element;
		/* copy
		virtual void Awake()   override;
		virtual void Start()   override;
		virtual void Tick()    override;
		virtual void Destroy() override;
		*/
	public:
		std::function<void(KeyCode)> OnMouseButtonDown;
		std::function<void(KeyCode)> OnMouseButton;
		std::function<void(KeyCode)> OnMouseButtonUp;


		std::function<void(KeyCode)> OnMouseClick;
		std::function<void(KeyCode)> OnMouseDoubleClick;


		std::function<void()> OnMouseEnter;
		std::function<void()> OnMouseMove;
		std::function<void()> OnMouseExit;

		std::function<void(KeyCode)> OnKeyDown;
		std::function<void(KeyCode)> OnKey;
		std::function<void(KeyCode)> OnKeyUp;

		std::function<void(float, float)> OnResize;

		std::function<void()> OnFocusEnter;
		std::function<void()> OnFocus;
		std::function<void()> OnFocusExit;

		std::function<void(const GUIDropItem&)> OnDropItem;
		std::function<void(wchar_t)> OnChar;
	};
}

