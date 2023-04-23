#pragma once
#include "Core.h"
#include "MenuTrees.h"

class WWidget;
class WWidgetComponent;

class HContextMenuBuilder
{
public:
	void AddSection(const PName& inSectionName, int32 inPriority);
	void AddContextMenuPath(const PName& inSectionName, const PName& inMenuPath, int32 inPriority = 0);
	void AddContextMenu(const PName& inSectionName, const PName& inMenuPath, const HMenuCommand& inMenuCommand, const HMenuAction& inBindAction, const HIsVisibleMenu& inIsVisibleMenu, const HIsEnableMenu& inIsEnableMenu, int32 inPriority = 0);
	const HContextMenuTrees& GetMenuTrees() const;

private:
	HContextMenuTrees _menuTrees;
};

class IContextMenuBuild
{

public:
	virtual void PushData(const HContextMenuTrees& inMenuTrees) = 0;
	virtual bool Build(const HGuid& inWidgetID) = 0;
	virtual void Reset() = 0;
};