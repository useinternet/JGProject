#pragma once





#include "GUI/JGUIComponent/JGUIElementComponent.h"



class Button;
class JGUICanvas;
class JGUIImageRenderer;
class TitleBar : public JGUIScriptComponent
{
protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Destroy() override;
	virtual void Tick(float tick) override;
public:
	void UnConnect();
	void Connect(const JVector2Int& delta);
	bool IsGrap();
private:
	void SettingByWindowFlags();
	void Setting();
	void MouseBtDown(const JGUIKeyDownEvent& e);
private:
	Button* m_CloseBt = nullptr;
	Button* m_MaxBt = nullptr;
	Button* m_MinBt = nullptr;
	JGUICanvas* m_TitleCanvas  = nullptr;
	JGUIImageRenderer* m_Image = nullptr;

	//
	bool m_IsGrap = false;
	bool m_IsUnConnect = false;
	JVector2Int m_Delta;
};