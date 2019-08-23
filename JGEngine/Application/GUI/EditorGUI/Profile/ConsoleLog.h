#pragma once
#include "GUI/EditorGUI/IGWindow.h"



namespace JE
{
	class ConsoleLog : public IGWindow
	{
	protected:
		virtual void OnLoad() override;
		virtual void OnGUI() override;
		virtual void OnEvent(Event& e) override;
		virtual void OnDestroy() override;

	};
}