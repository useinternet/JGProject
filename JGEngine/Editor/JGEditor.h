#pragma once




#include "GUI/JGUICore.h"
#include "Core.h"



JGUI_MAIN_WINFORM(JGEditor, JGUI_WindowFlag_MainWindow)
class JGEDITOR_API JGEditor : public JGUIForm
{
protected:
	virtual void Awake() override;
	virtual void Start() override;

private:
	void InitMainMenu();

private:
};
