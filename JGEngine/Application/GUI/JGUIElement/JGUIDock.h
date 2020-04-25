#pragma once
#include "GUI/JGUIObject/JGUIElement.h"
#include <set>
class JGUIForm;
class JGUICanvas;
class JGUIDock : public JGUIElement
{

protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Resize(const JGUIResizeEvent& e) override;
	virtual void Tick(const JGUITickEvent& e) override;

public:
	virtual void SetActive(bool active, bool is_hierarchy = false) override;
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
	struct DockInfo;
	class JGUIDockResizeLine : public JGUIElement
	{
		friend JGUIDock;
	protected:
		virtual void Awake() override;
		virtual void MouseMove(const JGUIMouseMoveEvent& e) override;
		virtual void MouseBtDown(const JGUIKeyDownEvent& e) override;
		virtual void Tick(const JGUITickEvent& e) override;
	public:
		void BindDock(int line_type, DockInfo* l_t_info, DockInfo* r_b_info);
		void UnBindDock(DockInfo* dock_info);
		void AddBindDock(DockDirection dir, DockInfo* dock_info);
		bool IsBindedDock(DockInfo* info);
		void Reset();
		int GetType() const
		{
			return m_LineType;
		}
		float GetLineThickness() const {
			return m_Thick;
		}
		void SizeUpdateByBindedWindow();
		JGUIRect TopArea();
		JGUIRect BottomArea();
		JGUIRect RightArea();
		JGUIRect LeftArea();
		
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


		std::map<DockDirection, std::set<JGUIDockResizeLine*>> m_ConnectedResizeLines;
		
		int   m_LineType = 0;
		float m_Thick = 4;
		bool  m_IsResizeing = false;
		JVector2Int m_PrevMousePos;
	};
	struct DockInfo
	{
		JGUIRect    rect;
		JGUIWindow* win = nullptr;
		std::map<DockDirection, JGUIDockResizeLine*> resizeLines;
	};
	
private:
	void Setting();
	void SettingDockInfo();
	// 다시 재구현
	void SettingDockInfo(
		int type,
		JGUIRect resize_rect, JGUIRect prev_rect, JGUIRect& next_resize_rect,
		JGUIDockResizeLine* owner,
		std::map<JGUIDockResizeLine*, bool>& check_line,
		std::map<JGUIWindow*, bool>& check_win);

	void DockExecuteByDirection(DockDirection dir, JGUIForm* dragging_win, DockInfo* containing_dock);
	void CreateResizeLineByDirection(DockDirection dir, DockInfo* l_t_info, DockInfo* r_b_info);
	void ArrangeDockInfo(DockInfo* remove_info);
private:
	class JGUIImage*   m_Docks[DD_Count];
	JGUIElement*       m_DockTarget = nullptr;
	float  m_DockDistance     = 30 + JGUI::Gap();
	JColor m_DefaultColor     = { 1.0f,1.0f,1.0f,0.5f };
	JColor m_InteractionColor = { 0.1f,0.1f,1.0f,0.5f };
	JGUIRect m_PrevRect;
	
	std::unordered_map<JGUIWindow*, DockInfo*> m_DockInfoMap;
	std::unordered_map<DockInfo*, std::unique_ptr<DockInfo>> m_DockInfoPool;
	std::set<JGUIDockResizeLine*> m_ResizeLinePool;
	JGUIElement* m_LineCanvas = nullptr;
	float m_DefaultResizeThick = 4;
};