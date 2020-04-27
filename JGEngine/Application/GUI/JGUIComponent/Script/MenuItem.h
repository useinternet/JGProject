#pragma once

#include "GUI/JGUIComponent/JGUIElementComponent.h"


// 단축키
// 자식 메뉴아이템 배경
// 보조키, 단축키
//

/*

Image
	ContentsText
	ShortcutKeyText
	Icon

ChildItems
	Item1..
	Item2..
	Item3..

Separaters
	Sep1..
	Sep2..

*/
class JGUIImageRenderer;
class JGUIShapeRenderer;
class Text;
class MainMenu;
class MainMenuItem;
class ContextMenu;
class MenuItemCollection;

class JGENGINE_API MenuItemDesc
{
public:
	
	std::vector<KeyCode> shortcut_key;
	std::function<void(void*)> func;

	std::string name;
	std::string contents;
	KeyCode     auxiliary_key = KeyCode::Alt;
	void*       data = nullptr;

	std::string icon_source_path  = "none";
	std::string image_source_path = "none";
	std::string backGround_path   = "none";
};

class JGENGINE_API MenuItem : public JGUIScriptComponent
{
	friend MenuItemCollection;
	friend MainMenu;
	friend MainMenuItem;
	friend ContextMenu;
	enum
	{
		MenuItemDefaultHeight = 20
	};
protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Destroy() override;
	virtual void Tick(float tick) override;
public:
	MenuItem* AddItem(const MenuItemDesc& desc);
	MenuItem* GetItem(uint32_t idx) const;
	uint32_t  GetItemCount() const;

	void      AddSeparater();

	void      OpenItems();
	void      CloseItems();
	bool      IsOpenItems() const;
private:
//private:
	void Setting();
	void ItemsSetting();
	void ItemOperation();


	void ReceiveAuxKeyDown(const JGUIKeyDownEvent& e);
	void ReceiveAuxKeyUp(const JGUIKeyUpEvent& e);

	void ReceiveShortCutKeyDown(const JGUIKeyDownEvent& e);
	void ReceiveShortCutKeyUp(const JGUIKeyUpEvent& e);
private:
	std::map<uint32_t, JGUIElement*> m_SepIndexMap;

	JGUIElement*           m_Items            = nullptr;
	JGUIElement*           m_Separaters       = nullptr;
	JGUIImageRenderer*     m_BackGround       = nullptr;
	JGUIShapeRenderer*     m_BackGroundBorder = nullptr;
	//
	JGUIImageRenderer*     m_Image           = nullptr;
	JGUIImageRenderer*     m_Icon            = nullptr;
	JGUIImageRenderer*     m_Arrow           = nullptr;
	//
	Text*                  m_ContentsText    = nullptr;
	Text*                  m_ShortcutKeyText = nullptr;
	MenuItemDesc           m_Desc;

	// 미세 조정 변수들
	float m_ContentsTextWidth    = 0.0f;
	float m_ShortcutKeyTextWidth = 0.0f;


	bool m_IsSetting = false;
	bool m_IsDirty   = true;


	bool m_IsMouseDown = false;


	// 단축키 변수
	bool  m_IsAuxOperation      = false;
	bool  m_IsShortCutOperation = false;
	bool  m_ActiveAuxKey     = false;


	std::map<KeyCode, bool> m_ActiveShortcutKey;
	uint32_t                m_ShortCutIndex = 0;





	std::string m_ArrowSource_Path = "Right_Arrow.png";
	MenuItemCollection* m_OwnerCollection = nullptr;
private:
	static const float IconGap;
	static const float ArrowGap;
	static const float SepHeight;
	static const float SepGap;
	static const float BorderThick;


	static const JColor NormalColor;
	static const JColor HightlightColor;
	static const JColor BorderColor;
	static const JColor SepColor;
};




class JGENGINE_API MenuItemCollection : public JGUIScriptComponent
{
protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Destroy() override;
	virtual void Tick(float tick) override;
public:
	MenuItem* AddItem(const MenuItemDesc& desc);
	MenuItem* GetItem(uint32_t idx) const;
	uint32_t  GetItemCount() const;

	void      AddSeparater();

	void      OpenItems();
	void      CloseItems();
	bool      IsOpenItems() const;
public:
	void ReceiveAuxKeyDown(const JGUIKeyDownEvent& e);
	void ReceiveAuxKeyUp(const JGUIKeyUpEvent& e);

	void ReceiveShortCutKeyDown(const JGUIKeyDownEvent& e);
	void ReceiveShortCutKeyUp(const JGUIKeyUpEvent& e);
private:
	void ItemsSetting();
	void ItemsAuxKeyUp(MenuItem* item, const JGUIKeyUpEvent& e);

	void ItemsShortcutKeyDown(MenuItem* item, const JGUIKeyDownEvent& e);
	void ItemsShortcutKeyUp(MenuItem* item, const JGUIKeyUpEvent& e);
private:
	std::map<uint32_t, JGUIElement*> m_SepIndexMap;
	JGUIImageRenderer* m_BackGround       = nullptr;
	JGUIShapeRenderer* m_BackGroundBorder = nullptr;
	JGUIElement*       m_Separaters       = nullptr;
	JGUIElement*       m_MenuItems        = nullptr;

	bool   m_IsDirty = true;
};