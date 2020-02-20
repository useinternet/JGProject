#pragma once
#include "GUI/JGUIObject/JGUIComponent.h"



struct JGUIMenuItemDesc
{
	std::string name = "JGUIMenuItem";
};

class JGUIMenuItem : public JGUIComponent
{
	friend class JGUIMenu;
	friend class JGUIContextMenu;
protected:
	virtual void Awake() override;
	virtual void Resize(const JGUIResizeEvent& e) override;
	virtual void Start() override;
	//virtual void MouseMove(const JGUIMouseMoveEvent& e) override;
	//virtual void MouseLeave() override;
public:
	JGUIMenuItem* AddMenuItem(const JGUIMenuItemDesc& desc);
	void SetDesc(const JGUIMenuItemDesc& desc);

	uint32_t      GetMenuItemCount() const;
	JGUIMenuItem* GetMenuItem(uint32_t idx);
private:
	void Setting();
	void SubItemOpen();
	void SubItemClose();
	void BtOnClick();
	void CreateRightArrow();
	bool CollisionCheck(const JVector2Int& mouse_pos);
private:
	enum EMenuItemType
	{
		MenuItemType_MainMenuItem,
		MenuItemType_SubMenuItem,
		MenuItemType_ContextMenuItem,
	};
	void SetMenuItemType(EMenuItemType type);
	void NotifySelectItem(JGUIMenuItem* item);
private:
	class JGUIButton* m_Bt = nullptr;
	class JGUIText* m_Text = nullptr;
	class JGUIImage* m_Image = nullptr;
	std::vector<JGUIMenuItem*> m_SubItems;
	JGUIMenuItem*              m_OwnerItem = nullptr;
	JGUIMenuItemDesc           m_Desc;



	JGUIMenu* m_OwnerMainMenu = nullptr;
	float m_Tick = 0.0f;
	float m_OpenTime = 0.5f;
	bool  m_IsOpen = false;

	EMenuItemType m_MIType = MenuItemType_SubMenuItem;
};