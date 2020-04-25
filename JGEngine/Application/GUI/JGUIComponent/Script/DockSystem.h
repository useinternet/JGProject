#pragma once


#include "GUI/JGUIComponent/JGUIElementComponent.h"
#include <set>
#include <map>
class DockSystem : public JGUIScriptComponent
{
	class DockResizeLine;
protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Destroy() override;
	virtual void Tick(float tick) override;
public:
	virtual void SetActive(bool active) override;
	void RequestRemoveBinedDock(JGUIWindow* win);

private:
	friend DockResizeLine;
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
		std::map<DockDirection, DockResizeLine*> resizeLines;
	};
private:
	void Setting();
	void SettingDockInfo();
	// 다시 재구현
	void SettingDockInfo(
		int type,
		JGUIRect resize_rect, JGUIRect prev_rect, JGUIRect& next_resize_rect,
		DockResizeLine* owner,
		std::map<DockResizeLine*, bool>& check_line,
		std::map<JGUIWindow*, bool>& check_win);

	void DockExecuteByDirection(DockDirection dir, JGUIWindow* dragging_win, DockInfo* containing_dock);
	void CreateResizeLineByDirection(DockDirection dir, DockInfo* l_t_info, DockInfo* r_b_info);
	void ArrangeDockInfo(DockInfo* remove_info);
private:
	using DockInfoPool = std::unordered_map<DockInfo*, std::unique_ptr<DockInfo>>;
	using DockInfoByWindow = std::unordered_map<JGUIWindow*, DockInfo*>;

	class JGUIImageRenderer* m_Docks[DD_Count];
	JGUIElement*             m_DockTarget = nullptr;
	float                    m_DockDistance = 30 + JGUI::Gap();
	JColor                   m_DefaultColor = { 1.0f,1.0f,1.0f,0.5f };
	JColor                   m_InteractionColor = { 0.1f,0.1f,1.0f,0.5f };
	JGUIRect                 m_PrevRect;
	//
	DockInfoByWindow          m_DockInfoMap;
	DockInfoPool              m_DockInfoPool;
	std::set<DockResizeLine*> m_ResizeLinePool;
	//
	class JGUICanvas* m_LineCanvas = nullptr;
	float             m_DefaultResizeThick = 4;
	float             m_DockRectSize = 30.0f;



	class DockResizeLine : public JGUIScriptComponent
	{
		friend DockSystem;
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Destroy() override;
		virtual void Tick(float tick) override;
	public:
		void BindDock(int line_type, DockInfo* l_t_info, DockInfo* r_b_info);
		void UnBindDock(DockInfo* dock_info);
		void AddBindDock(DockDirection dir, DockInfo* dock_info);
		bool IsBindedDock(DockInfo* info);
		void Reset();

		//
		void SizeUpdateByBindedWindow();


		//
		JGUIRect TopArea();
		JGUIRect BottomArea();
		JGUIRect RightArea();
		JGUIRect LeftArea();




		int GetType() const
		{
			return m_LineType;
		}
		float GetLineThickness() const {
			return m_Thick;
		}
	private:
		enum
		{
			HLine,
			VLine
		};
	private:
		class JGUIImageRenderer* m_Line = nullptr;

		//
		std::map<JGUIWindow*, DockInfo*> m_LeftDock;
		std::map<JGUIWindow*, DockInfo*> m_RightDock;
		std::map<JGUIWindow*, DockInfo*> m_TopDock;
		std::map<JGUIWindow*, DockInfo*> m_BottomDock;


		std::map<DockDirection, std::set<DockSystem::DockResizeLine*>> m_ConnectedResizeLines;

		int   m_LineType = 0;
		float m_Thick = 4;
		bool  m_IsResizeing = false;
		JVector2Int m_PrevMousePos;
	};



};



