#pragma once
#include "GUI/JGUIObject/JGUIElement.h"
#include "JGUIButton.h"
class JGUIImage;
class JGUIScrollbar : public JGUIElement
{

protected:
	virtual void Awake() override;
	virtual void Resize(const JGUIResizeEvent& e) override;
	void ElementSetting();
public:
	void SetBarType(int n);
	int GetBarType() const {
		return m_BarType;
	}
	void Set(float occupyRatio);


	float GetCurrentRatio() const {
		return m_Ratio;
	}
	float GetOccupyRatio() const {
		return m_OccupyRatio;
	}
	void AdjustBar(float ratio);
public:
	enum
	{
		Horizontal,
		Vertical
	};
protected:
	class JGUIButtonBar : public JGUIButton
	{
		friend JGUIScrollbar;
	protected:
		virtual void Awake() override;
		virtual void MouseBtDown(const JGUIKeyDownEvent& e) override;
	private:
		bool m_IsGrap = false;
		JVector2Int m_PrevMousePos;
		float m_PrevRatio = 0.0f;
		JGUIScrollbar* m_Owner = nullptr;
	};
private:

private:
	JGUIImage* m_ScrollBackGround = nullptr;
	JGUIButtonBar* m_Bar = nullptr;


	float m_Ratio       = 0.0f;
	float m_OccupyRatio = 0.2f;
	float m_TotalRatio  = 1.0f;




	float m_BarSize  = 25.0f;
	int   m_BarType  = 0;
};