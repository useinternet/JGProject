#include "PCH/PCH.h"
#include "ContextMenuBuilder.h"
#include "Widget.h"
#include "WidgetComponents/WidgetComponent.h"

void HContextMenuBuilder::AddSection(const PName& inSectionName, int32 inPriority)
{
	HContextMenuTrees::HSectionData sectionData;
	sectionData.Name = inSectionName;
	sectionData.Priority = inPriority;

	_menuTrees._sectionDatas.push_back(sectionData);
}

void HContextMenuBuilder::AddContextMenuPath(const PName& inSectionName, const PName& inMenuPath, int32 inPriority)
{
	_menuTrees.createMenuPathIfExist(inSectionName, inMenuPath.ToString(), inPriority, INDEX_NONE);
}

void HContextMenuBuilder::AddContextMenu(const PName& inSectionName, const PName& inMenuPath, const HMenuCommand& inMenuCommand, const HMenuAction& inBindAction, const HIsVisibleMenu& inIsVisibleMenu, const HIsEnableMenu& inIsEnableMenu, int32 inPriority)
{
	_menuTrees.createMenuPathIfExist(inSectionName, inMenuPath.ToString(), inPriority, (int32)_menuTrees._menuDatas.size());

	HContextMenuTrees::HMenuData menuData;
	menuData.SectionName = inSectionName;
	menuData.MenuCommand = inMenuCommand;
	menuData.Action		 = inBindAction;
	menuData.IsVisible	 = inIsVisibleMenu;
	menuData.IsEnable	 = inIsEnableMenu;
	menuData.Priority    = inPriority;

	_menuTrees._menuDatas.push_back(menuData);
}
const HContextMenuTrees& HContextMenuBuilder::GetMenuTrees() const
{
	return _menuTrees;
}
