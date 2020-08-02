#pragma once

#include "Element.h"

namespace GUI
{
	class GUI_API Text : public Element
	{
	protected:
		virtual void Awake()   override;
		virtual void Start()   override;
		virtual void Tick()    override;
		virtual void Destroy() override;

	private:
		class TextMeshComponent* m_TMCom = nullptr;
	};
}