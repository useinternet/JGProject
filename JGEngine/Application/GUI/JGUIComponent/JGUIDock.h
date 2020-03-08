#pragma once
#include "GUI/JGUIObject/JGUIComponent.h"
class JGUIForm;


// 1. 추가 하는건 완료
// 2. 윈도우가 Dock 시스템에서 나가거나 삭제 될때 사각형 합병 구현
class JGUIDock : public JGUIComponent
{
protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Resize(const JGUIResizeEvent& e) override;
	virtual void Tick(const JGUITickEvent& e) override;

public:
	void RequestRemoveBinedDock(JGUIWindow* win);
private:
	enum DockDirection
	{
		DD_Left,
		DD_Right,
		DD_Center,
		DD_Top,
		DD_Bottom,
		DD_Count
	};
	struct DockInfo
	{
		JGUIRect    rect;
		JGUIWindow* win = nullptr;
	};
	class JGUIDockResizeLine : public JGUIComponent
	{
	protected:
		virtual void Awake() override;
		virtual void MouseMove(const JGUIMouseMoveEvent& e) override;
		virtual void MouseLeave() override;
		virtual void MouseBtDown(const JGUIKeyDownEvent& e) override;
	public:
		void BindDock(int line_type, DockInfo* l_t_info, DockInfo* r_b_info);
		void UnBindDock(DockInfo* dock_info);
		void AddBindDock(DockInfo* dock_info);
		bool IsBindedDock(DockInfo* info);
		void Reset();
	private:
		enum
		{
			HLine,
			VLine
		};
	private:
		class JGUIRectangle* m_Line = nullptr;

		//
		std::map<JGUIWindow*, DockInfo*> m_LeftDock;
		std::map<JGUIWindow*, DockInfo*> m_RightDock;
		std::map<JGUIWindow*, DockInfo*> m_TopDock;
		std::map<JGUIWindow*, DockInfo*> m_BottomDock;
		int   m_LineType = 0;
		float m_Thick = JGUI::Gap();
		bool  m_IsResizeing = false;
		JVector2Int m_PrevMousePos;
	};
private:
	void Setting();
	void SettingDockInfo();
	void DockExecuteByDirection(DockDirection dir, JGUIForm* dragging_win, DockInfo* containing_dock);
	void CreateResizeLineByDirection(DockDirection dir, DockInfo* l_t_info, DockInfo* r_b_info);
	void ArrangeDockInfo(DockInfo* remove_info);
private:
	class JGUIRectangle* m_Docks[DD_Count];
	JGUIComponent*       m_DockTarget = nullptr;
	float  m_DockDistance     = 30 + JGUI::Gap();
	JColor m_DefaultColor     = { 1.0f,1.0f,1.0f,0.5f };
	JColor m_InteractionColor = { 0.1f,0.1f,1.0f,0.5f };
	JGUIRect m_PrevRect;
	std::unordered_map<JGUIWindow*, DockInfo*> m_DockInfoMap;
	std::unordered_map<DockInfo*, std::unique_ptr<DockInfo>> m_DockInfoPool;
};