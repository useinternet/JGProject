#pragma once



#include "GUI/JGUIComponent/JGUIElementComponent.h"
#include "GUI/JGUIComponent/Script/Button.h"

class JGUIImageRenderer;
class Scrollbar : public JGUIScriptComponent
{
public:
	enum
	{
		Horizontal,
		Vertical
	};
private:
	class ScrollButton;
protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Tick(float tick) override;
	virtual void Destroy() override;
public:
	void AdjustBar(float ratio);
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
private:
	void Setting();
private:
	JGUIImageRenderer* m_ScrollBackGround = nullptr;
	ScrollButton*      m_ScrollBt = nullptr;


	float m_Ratio = 0.0f;
	float m_OccupyRatio = 0.2f;
	float m_TotalRatio = 1.0f;




	float m_BarSize = 25.0f;
	int   m_BarType = 0;
private:
	class ScrollButton : public Button
	{
		friend Scrollbar;
	protected:
		virtual void Awake() override;
		virtual void Start() override;

	private:
		bool        m_IsGrap = false;
		JVector2Int m_PrevMousePos;
		float       m_PrevRatio = 0.0f;
		Scrollbar*  m_OwnerScrollbar = nullptr;
	};


};

