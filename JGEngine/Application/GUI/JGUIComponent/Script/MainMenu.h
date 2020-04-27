#pragma once


#include "GUI/JGUIComponent/JGUIElementComponent.h"


class MenuItem;
class MenuItemDesc;
class MenuItemCollection;

class MainMenuItem;
class JGUIImageRenderer;

class Text;

class JGENGINE_API MainMenuItemDesc
{
public:
	std::string name;
	std::string contents;
	KeyCode     auxiliary_key = KeyCode::Alt;
	std::string image_source_path = "none";
	std::string backGround_path = "none";
};

class JGENGINE_API MainMenu : public JGUIScriptComponent
{
protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Destroy() override;
	virtual void Tick(float tick) override;
public:
	MainMenuItem* AddItem(const MainMenuItemDesc& desc);
	MainMenuItem* GetItem(uint32_t idx) const;
	uint32_t      GetItemCount() const;

public:
	void ReceiveKeyDown(const JGUIKeyDownEvent& e);
	void ReceiveKeyUp(const JGUIKeyUpEvent& e);
private:
	void Setting();
	bool CheckPoint(MenuItem* item);
private:
	JGUIElement*       m_MainMenuItems = nullptr;
	JGUIImageRenderer* m_Image   = nullptr;
};



class JGENGINE_API MainMenuItem : public JGUIScriptComponent
{
	friend MainMenu;
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
private:
	void Setting();
	void ReceiveAuxKeyDown(const JGUIKeyDownEvent& e);
	void ReceiveAuxKeyUp(const JGUIKeyUpEvent& e);
private:
	JGUIImageRenderer*  m_Image = nullptr;
	MenuItemCollection* m_ItemCollection = nullptr;

	float m_ContentsTextWidth = 0.0f;
	Text* m_ContentsText      = nullptr;
	bool  m_IsMouseDown       = false;

	//
	bool m_ActiveAuxKey   = false;



	MainMenuItemDesc m_Desc;
};

