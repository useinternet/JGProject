#pragma once
#include "Core.h"
#include "GUIDefine.h"
#include "Builder/ContextMenuBuilder.h"


class PImGuiContextMenuBuild final : public IMemoryObject, public IContextMenuBuild
{
public:
	virtual ~PImGuiContextMenuBuild() = default;

private:
	virtual void PushData(const HContextMenuTrees& inMenuTrees) override;
	virtual bool Build(const HGuid& inWidgetID) override;
	virtual void Reset() override;

private:
	bool onBeginContextMenu(const PName& menuName);
	void onEndContextMenu();
	void onContextMenuItem(const HContextMenuTrees::HMenuData& inMenuItem);
	void onContextMenuAction(const HContextMenuTrees::HMenuData& inMenuItem);

	bool isPressedShortcut(uint64 inWidgetID, const HMenuShortcut& inShortcut) const;

private:
	HContextMenuTrees _menuTrees;
	bool bTreeDirty;
};