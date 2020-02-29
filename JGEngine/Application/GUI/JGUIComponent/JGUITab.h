#pragma once

#include "GUI/JGUIObject/JGUIComponent.h"
#include "GUI/JGUIComponent/JGUIButton.h"
class JGUIWindow;
class JGUIText;
class JGUITabButton;
class JGUITab : public JGUIComponent
{
	friend JGUITabButton;
protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Resize(const JGUIResizeEvent& e) override;
	virtual void Tick(const JGUITickEvent& e) override;

	virtual void MouseMove(const JGUIMouseMoveEvent& e) override;
	virtual void MouseLeave() override;
	virtual void Destroy() override;

public:
	void BindWindow(JGUIWindow* win);
	void ActiveTab(JGUIWindow* win);
private:
	void ReAdjustTabBtPos();
private:
	struct TabInfo
	{
		JGUITabButton* bt  = nullptr;
		JGUIWindow* win = nullptr;
		JGUIText*   txt = nullptr;
	};
private:
	class JGUIRectangle*  m_Background = nullptr;
	class JGUIPanel*      m_ClientPanel = nullptr;
	JGUITabButton* m_PressedTabBt = nullptr;
	JVector2Int m_MousePosWhenPressed;
	std::vector<TabInfo>  m_TabInfo;
};


class JGUITabButton : public JGUIButton
{
	friend JGUITab;
protected:
	virtual void Awake() override;
	virtual void OnClick() override;
	virtual void MouseBtUp(const JGUIKeyUpEvent& e) override;
	virtual void MouseBtDown(const JGUIKeyDownEvent& e) override;
private:
	JGUIWindow* m_BindedWindow = nullptr;
	JGUITab*    m_OwnerTab = nullptr;
};


// Tab 구현 세부 내용


/*
1. 현재 보여주는 탭 상태

- 탭에 바인딩된 윈도우는 현재 보여주는 윈도우의 자식이다.
- 활성화된 탭을 제외하고 다른 탭은 우선순위를 낮게 설정하거나 비활성화한다.
- 


2. 탭이동
1. 타이틀 바 드래그 시 JGUI한테 윈도우 정보를 저장
2. 커서 위치


// -- 세부 사항
-- 텍스트 하얀색으로
-- 버튼색 비쥬얼스튜디오 스포이드로 따서 색 선정
-- 탭으로도 이동 가능하게 
-- 타이틀바가 생기면서 그대로 이동가능하게 (타이틀바 생길때 버튼 크기에 비례해서 타이틀바 잡은 위치 계산)
-- 탭주변에 있으면 타이틀바가 버튼으로 되면서 버튼 이동( 버튼도 생길대 타이틀바 잡은 위치 비례해서 위치 계산)

*/