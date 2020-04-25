#pragma once
#include "GUI/JGUIObject/JGUIElement.h"



struct JGUIMenuItemDesc
{
	std::string name = "JGUIMenuItem";

	bool    is_use_keyboardkey = false;
	KeyCode keyboardKey;

	bool is_use_shortcut_key = false;
	bool is_ctrl  = false;
	bool is_shift = false;
	bool is_alt   = false;
	KeyCode shortcut_key;


	bool is_binding_func = false;
	std::function<void()> func;
};

class JGUIMenuItem : public JGUIElement
{
	friend class JGUIMenu;
	friend class JGUIContextMenu;
protected:
	virtual void Awake() override;
	virtual void Resize(const JGUIResizeEvent& e) override;
	virtual void Start() override;
public:
	JGUIMenuItem* AddMenuItem(const JGUIMenuItemDesc& desc);
	void          AddSeparator();
	uint32_t      GetMenuItemCount() const;
	JGUIMenuItem* GetMenuItem(uint32_t idx);
	const JGUIMenuItemDesc& GetDesc() const {
		return m_Desc;
	}
private:
	void Setting();
	void SubItemOpen();
	void SubItemClose();
	void BtOnClick();
	void CreateRightArrow();
	bool CollisionCheck(const JVector2Int& mouse_pos);
	void SetDesc(const JGUIMenuItemDesc& desc);
	std::vector<JGUIMenuItem*> GetLastActivedMenuItems() const;
	void ProcessShortCutKey(bool is_ctrl, bool is_alt, bool is_shift, KeyCode code);
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
	class JGUIRectangle* m_BackGround = nullptr;
	class JGUIButton*    m_Bt         = nullptr;
	class JGUIText*      m_NameText   = nullptr;
	class JGUIText* m_ShortCutText = nullptr;
	class JGUIImage*     m_Image      = nullptr;
	std::vector<JGUIMenuItem*>    m_SubItems;
	std::map<int, JGUIRectangle*> m_Separators;

	JGUIMenuItem*              m_OwnerItem = nullptr;
	JGUIMenuItemDesc           m_Desc;

	float m_SeparatorHeightGap = 10.0f;
	float m_SeparatorWidthGap  = 8.0f;
	float m_SeparatorThickness = 1.0f;





	float m_Tick = 0.0f;
	float m_OpenTime = 0.5f;
	float m_SubItemNameWidth     = 0.0f;
	float m_SubItemShortCutWidth = 0.0f;




	float m_SubItemWidth = 0.0f;
	bool  m_IsOpen = false;

	
	JGUIMenu*     m_OwnerMainMenu = nullptr;
	EMenuItemType m_MIType = MenuItemType_SubMenuItem;

	
};