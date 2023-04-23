#include "PCH/PCH.h"
#include "MenuBuilder.h"

void HMenuBuilder::AddSection(const PName& inSectionName, int32 inPriority)
{
	HMenuTrees::HSectionData sectionData;
	sectionData.Name = inSectionName;
	sectionData.Priority = inPriority;

	_mainMenuTrees._sectionDatas.push_back(sectionData);
}

void HMenuBuilder::AddMenuPath(const PName& inSectionName, const PName& inMenuPath, int32 inPriority)
{
	_mainMenuTrees.createMenuPathIfExist(inSectionName, inMenuPath.ToString(), inPriority, INDEX_NONE);
}

void HMenuBuilder::AddMainMenu(const PName& inSectionName, const PName& inMenuPath, const HMenuCommand& inMenuCommand, const HMenuAction& inBindAction, const HIsVisibleMenu& inIsVisibleMenu, const HIsEnableMenu& inIsEnableMenu, int32 inPriority)
{
	_mainMenuTrees.createMenuPathIfExist(inSectionName, inMenuPath.ToString(), inPriority, (int32)_mainMenuTrees._mainMenuDatas.size());

	HMenuTrees::HMainMenuData mainMenuData;
	mainMenuData.SectionName = inSectionName;
	mainMenuData.MenuCommand = inMenuCommand;
	mainMenuData.Action      = inBindAction;
	mainMenuData.IsVisible   = inIsVisibleMenu;
	mainMenuData.IsEnable    = inIsEnableMenu;

	_mainMenuTrees._mainMenuDatas.push_back(mainMenuData);
}

void HMenuBuilder::AddMenu(const PName& inSectionName, PWeakPtr<WWidget> inWidget, const HMenuCommand& inMenuCommand, const HMenuAction& inBindAction)
{
	HMenuTrees::HMenuData menuData;
	menuData.SectionName  = inSectionName;
	menuData.Widget		  = inWidget;
	menuData.MenuCommand  = inMenuCommand;
	menuData.Action       = inBindAction;

	_mainMenuTrees._menuDatas.push_back(menuData);
}

const HMenuTrees& HMenuBuilder::GetMenuTrees() const
{
	return _mainMenuTrees;
}


