#pragma once
#include "GUI/JGUIComponent/JGUIElementComponent.h"
#include "GUI/JGUIComponent/Script/Button.h"
class Button;
class Text;
class TabSystem : public JGUIScriptComponent
{
	class TabButton;
	friend TabButton;

protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Destroy() override;
	virtual void Tick(float tick) override;
public:
	void ActiveWindow(JGUIWindow* win);
	void BindWindow(JGUIWindow* win);
	void BindWindowByCursorPos();
	void AdjustTabWindowSize();
private:

	struct TabInfo
	{
		Text* txt = nullptr;
		TabButton* bt      = nullptr;
		JGUIWindow* win = nullptr;
		std::string origin_name;
	};





private:
	TabInfo* CreateTabInfo(JGUIWindow* win);
	void NotifyDraggingTabButton(Button* tabBt);
	void DraggingTabBtExtraEvent();
	void ArrangeTab();
	int  GetTabIndex(Button* bt);
	int  GetTabIndex(const JVector2Int& p);
	void SwapTab(int tn1, int tn2);
	void DeleteTab(int idx);
	void ArrangeBtSizeByTabCount();
	JGUIWindow* FindWindowByBt(Button* tabBt);
private:
	std::vector<std::unique_ptr<TabInfo>> m_TabInfos;
	Button* m_CurrentDraggingTabBt = nullptr;
	Button* m_MainTabBt            = nullptr;


	JGUIWindow* m_DraggingWindow = nullptr;

	//
	float  m_TabLineHeight = 2.0f;
	float  m_BtTextSize = 15.0f;

	JColor m_ActiveColor;
	JColor m_UnActiveColor;

	bool   m_SwapLock = false;
	// 
	int    m_PressedBtIndex =  -1;
	//
	int   m_DeltaXDistance = 0;
	//float m_TitleSpareWidth = 0.0f;
	float m_TabGap = 100.0f;
	float m_TabBtWidth = 150.0f;



private:
	class TabButton : public Button
	{
		virtual void Start() override;
	public:
		TabSystem*   m_OwnerTab = nullptr;
		JGUIWindow* m_BindedWindow = nullptr;

	};
};