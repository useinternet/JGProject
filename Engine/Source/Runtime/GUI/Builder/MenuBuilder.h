#pragma once
#include "Core.h"
#include "GUIDefine.h"
#include "MenuTrees.h"

class WWidget;
class HMenuBuilder
{
public:
	void AddSection(const PName& inSectionName, int32 inPriority);
	void AddMenuPath(const PName& inSectionName, const PName& inMenuPath, int32 inPriority = 0);
	void AddMainMenu(const PName& inSectionName, const PName& inMenuPath, const HMenuCommand& inMenuCommand, const HMenuAction& inBindAction, const HIsVisibleMenu& inIsVisibleMenu, const HIsEnableMenu& inIsEnableMenu, int32 inPriority = 0);
	void AddMenu(const PName& inSectionName, PWeakPtr<WWidget> inWidget, const HMenuCommand& inMenuCommand, const HMenuAction& inBindAction);

	const HMenuTrees& GetMenuTrees() const;
private:
	HMenuTrees _mainMenuTrees;
};

class IMenuBuild
{
public:
	virtual void PushData(const HMenuTrees& inMenuTrees) = 0;
	virtual void Build() = 0;
	virtual void Reset() = 0;
};

