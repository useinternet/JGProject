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
class MenuItemDesc
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

class MenuItem : public JGUIScriptComponent
{
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
	MenuItem* GetItem(const std::string& name) const;
	uint32_t  GetItemCount() const;

	void      AddSeparater();

	void      OpenItems();
	void      CloseItems();
	bool      IsOpenItems() const;
//private:
	void Setting();
	void ItemsSetting();
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

public:
	MenuItemDesc           m_Desc;

	// 미세 조정 변수들
	float m_ContentsTextWidth    = 0.0f;
	float m_ShortcutKeyTextWidth = 0.0f;
	float m_IconGap   = 1.0f;
	float m_ArrowGap  = 4.0f;
	float m_SepHeight = 5.0f;
	float m_SepGap    = 2.0f;
	float m_BorderThick = 0.5f;

	bool m_IsSetting = false;
	bool m_IsDirty   = true;


	bool m_IsMouseDown = false;



	JColor m_NormalColor      = JColor(0.106f, 0.106f, 0.116f, 1.0f);
	JColor m_HightlightColor  = JColor(0.206f, 0.206f, 0.216f, 1.0f);
	JColor m_BorderColor      = JColor(0.206f, 0.206f, 0.216f, 1.0f);
	JColor m_SepColor         = JColor(0.376f, 0.376f, 0.386f, 1.0f);

	std::string m_ArrowSource_Path = "Right_Arrow.png";
};