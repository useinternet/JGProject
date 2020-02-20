#pragma once
#include "GUI/JGUIObject/JGUIComponent.h"
#include "JGUIMenuItem.h"


class JGUIMenu : public JGUIComponent
{
	friend void JGUIMenuItem::BtOnClick();
protected:
	virtual void Awake() override;
	virtual void Tick(const JGUITickEvent& e) override;

public:
	JGUIMenuItem* AddMenuItem(const JGUIMenuItemDesc& desc);
	uint32_t      GetMenuItemCount() const;
	JGUIMenuItem* GetMenuItem(uint32_t idx);
private:
	void NotifySelectItem(JGUIMenuItem* item);
	void MenuItemAllClose();
private:
	class JGUIRectangle*       m_Rectangle = nullptr;
	std::vector<JGUIMenuItem*> m_MenuItems;
};