#pragma once

#include "GUI/JGUIObject/JGUIWindow.h"


JGUI_MAIN_WINFORM(JGUIForm)


// 하위에서 상위로 갈때 윈도우 위치 는 괜찬은데 상위에서 하위로 갈때 위치계산이 이상함
// Dock은 Dragging 중이고 윈도우 Panel에 부딪혔을때만 Dock 키기
// 윈도우크기가 변하면 Dock에 있는 윈도우 크기도 변경
// Left나 Right 중 하나라도 채워지면 위아래 할때 아래쪽 불가
// 위 아래 중 하나라도 채워지면 오른쪽 왼쪽 불가
class JGUIForm : public JGUIWindow
{
	class JGUITitleBar*       m_Title     = nullptr;
	class JGUIResizeBox*      m_ResizeBox = nullptr;
	class JGUIEmptyRectangle* m_Border    = nullptr;
	class JGUIMenu*           m_Menu      = nullptr;
	class JGUIDock*           m_Dock      = nullptr;
	bool m_DockSwitch = false;
//	class JGUITab*            m_Tab       = nullptr;

	//TestCode
	class JGUIText* m_Txt = nullptr;
	class JGUIEmptyRectangle* m_text = nullptr;
	
	JGUIWindow* testwindow = nullptr;
protected:
	virtual void Awake() override;
	virtual void Tick(const JGUITickEvent& e) override;

	virtual void Resize(const JGUIResizeEvent& e) override;
	virtual void KeyDown(const JGUIKeyDownEvent& e) override;
	virtual void KeyUp(const JGUIKeyUpEvent& e) override;
	virtual void ProcessByWindowFlags(EJGUI_WindowFlags flag) override;
public:
	JGUITitleBar* GetTitleBar() const;
	JGUIDock* GetDock() const;
//	JGUITab* GetTab() const;
	JGUIRect GetClientRect() const;
	void DockOn();
	void DockOff();
};





