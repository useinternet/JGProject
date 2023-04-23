#pragma once
#include "Core.h"
#include "GUIDefine.h"
#include "Builder/MenuBuilder.h"



class PImGuiMenuBuild final : public IMemoryObject, public IMenuBuild
{
public:
	virtual ~PImGuiMenuBuild() = default;


private:
	virtual void PushData(const HMenuTrees& inMenuTrees) override;
	virtual void Build() override;
	virtual void Reset() override;

private:
	bool onBeginMainMenu(const PName& menuName);
	void onEndMainMenu();
	void onMainMenuItem(const HMenuTrees::HMainMenuData& inMainMenuItem);
	void onMainMenuAction(const HMenuTrees::HMainMenuData& inMainMenuItem);
	void onMenuAction(const HMenuTrees::HMenuData& inMenuItem);

	bool isPressedShortcut(uint64 inWidgetID, const HMenuShortcut& inShortcut) const;
private:
	HMenuTrees _menuTrees;
	bool bTreeDirty;
};