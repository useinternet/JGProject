#pragma once

#include "Core.h"
/*
1. Editor 연결
2. SceneImage UI 제작
3. 


*/


REGISTER_MAIN_WINFORM(JGEditor, L"JGEngine", GUI::WindowFlag_MainWindow)
class JGEDITOR_API JGEditor : public GUI::Form
{
protected:
	virtual void Awake()   override;
	virtual void Start()   override;
	virtual void Tick()    override;
	virtual void Destroy() override;
protected:
	virtual void OnKeyUp(KeyCode code) override;
};
