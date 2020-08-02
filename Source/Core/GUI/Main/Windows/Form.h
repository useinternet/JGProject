#pragma once

#include "Window.h"


namespace GUI
{
	
	class GUI_API Form : public Window
	{
	protected:
		virtual void Awake()   override {}
		virtual void Start()   override {}
		virtual void Tick()    override {}
		virtual void Destroy() override {}
	};
}



