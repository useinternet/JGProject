#pragma once
#include "GUI/JGUIObject/JGUIElement.h"
#include "JGUIMenuItem.h"


class JGUIMenu : public JGUIElement
{
	friend void JGUIMenuItem::BtOnClick();
protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Tick(const JGUITickEvent& e) override;
public:
	JGUIMenuItem* AddMenuItem(const JGUIMenuItemDesc& desc);
	uint32_t      GetMenuItemCount() const;
	JGUIMenuItem* GetMenuItem(uint32_t idx);

	void ReceiveKeyEvent(const JGUIKeyDownEvent& e);
	void ReceiveKeyEvent(const JGUIKeyUpEvent& e);
	void KeyFlush();
private:
	void NotifySelectItem(JGUIMenuItem* item);
	void MenuItemAllClose();
private:
	struct MenuKeyInfo
	{
		bool  is_pressed = false;
		bool  is_released = false;
		float tick       = 0.0f;
		float allow_time = 0.5f;
	};
private:
	class JGUIRectangle*       m_Rectangle = nullptr;
	std::vector<JGUIMenuItem*> m_MenuItems;


	float m_ItemWidth = 0.0f;


	//
	std::map<KeyCode, MenuKeyInfo> m_KeyInfoMap;
	std::map<KeyCode, bool>        m_KeyPressedMap;
};