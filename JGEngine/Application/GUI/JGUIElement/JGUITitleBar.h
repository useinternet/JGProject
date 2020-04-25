#pragma once
#include "GUI/JGUIObject/JGUIElement.h"
#include "GUI/JGUIElement/JGUIButton.h"





class JGUIPanel;
class JGUIText;
class JGUITextMesh;
class JGUITitleBar : public JGUIElement
{
protected:
	virtual void Awake() override;
	virtual void Resize(const JGUIResizeEvent& e) override;
	virtual void MouseBtDown(const JGUIKeyDownEvent& e) override;
	virtual void MouseBtUp(const JGUIKeyUpEvent& e) override;
	virtual void Tick(const JGUITickEvent& e) override;
public:
	void UnConnect();
	void Connect(const JVector2Int& delta);
private:
	void SettingTitleElementByWindowFlag();
	void SettingTitleElement();
	void TitleMouseBtDown(const JGUIKeyDownEvent& e);
private:
	// Title 관련
	JGUIPanel*          m_Panel   = nullptr;
	JGUICloseButton*    m_CloseBt = nullptr;
	JGUIMaximizeButton* m_MaxBt   = nullptr;
	JGUIMinimizeButton* m_MinBt   = nullptr;
	bool m_IsGrap = false;
	JVector2Int m_Delta;
	bool m_IsUnConnect = false;
	bool m_IsDraggingTabBtEvent = false;
public:
	void ActiveWindow(JGUIWindow* win);
	void BindWindow(JGUIWindow* win);
	void BindWindowByCursorPos();
	void AdjustTabWindowSize();
private:
	class JGUITabButton : public JGUIButton
	{
		friend JGUITitleBar;
	protected:
		virtual void MouseBtDown(const JGUIKeyDownEvent& e) override;
	private:
		JGUIWindow*   m_BindedWindow = nullptr;
		JGUITitleBar* m_Owner = nullptr;
	};
	// Tab 관련
	struct TabInfo
	{
		JGUITabButton*  bt  = nullptr;
		JGUIText*       txt = nullptr;
		JGUIWindow*    win = nullptr;
		std::string origin_name;
	};

private:
	TabInfo* CreateTabInfo(JGUIWindow* win);
	void TabTick(const JGUITickEvent& e);
	void NotifyDraggingTabButton(JGUITabButton* tabBt);
	void DraggingTabBtExtraEvent();
	void ArrangeTab();
	int  GetTabIndex(JGUITabButton* bt);
	int  GetTabIndex(const JVector2Int& p);
	void SwapTab(int tn1, int tn2);
	void DeleteTab(int idx);
	void ArrangeBtSizeByTabCount();
private:
	std::vector<std::unique_ptr<TabInfo>> m_TabInfos;
	JGUITabButton* m_CurrentDraggingTabBt = nullptr;
	JGUITabButton* m_MainTabBt = nullptr;
	JGUIWindow* m_DraggingWindow = nullptr;
	float  m_TabLineHeight = 2.0f;
	float  m_BtTextSize    = 15.0f;
	JColor m_ActiveColor;
	JColor m_UnActiveColor;
	bool   m_SwapLock = false;
	// 
	JVector2Int m_PrevMousePos;
	int m_DeltaXDistance = 0;
	float m_TitleSpareWidth = 0.0f;
	float m_TabGap = 100.0f;
	float m_TabBtWidth = 150.0f;
};
