#include "PCH/PCH.h"
#include "ImGuiContextMenuBuild.h"
#include "GUI.h"
#include "Widget.h"
#include "ImGuiBuildUtility.h"

void PImGuiContextMenuBuild::PushData(const HContextMenuTrees& inMenuTrees)
{
	_menuTrees = inMenuTrees;
	bTreeDirty = true;
}

bool PImGuiContextMenuBuild::Build(const HGuid& inWidgetID)
{
	if (bTreeDirty)
	{
		HContextMenuTrees::HEvents bindEvents;
		bindEvents.OnBeginContextMenu  = HContextMenuTrees::HOnBeginContextMenu::CreateSP(SharedWrap(this), &PImGuiContextMenuBuild::onBeginContextMenu);
		bindEvents.OnEndContextMenu    = HContextMenuTrees::HOnEndContextMenu::CreateSP(SharedWrap(this), &PImGuiContextMenuBuild::onEndContextMenu);
		bindEvents.OnContextMenuItem   = HContextMenuTrees::HOnContextMenuItem::CreateSP(SharedWrap(this), &PImGuiContextMenuBuild::onContextMenuItem);
		bindEvents.OnContextMenuAction = HContextMenuTrees::HOnContextMenuAction::CreateSP(SharedWrap(this), &PImGuiContextMenuBuild::onContextMenuAction);

		_menuTrees.BindEvents(bindEvents);
		bTreeDirty = false;
	}

	PSharedPtr<WWidget> findedWidget = GGUIGlobalSystem::GetInstance().FindWidget<WWidget>(inWidgetID);
	if (findedWidget != nullptr)
	{
		if (ImGui::BeginPopupContextWindow(inWidgetID.ToString().GetCStr()))
		{
			_menuTrees.UpdateTrees();

			ImGui::EndPopup();
			return true;
		}
	}
	else
	{
		if (ImGui::BeginPopupContextItem(inWidgetID.ToString().GetCStr()))
		{
			_menuTrees.UpdateTrees();

			ImGui::EndPopup();
			return true;
		}
	}

	return false;
}

void PImGuiContextMenuBuild::Reset()
{

}

bool PImGuiContextMenuBuild::onBeginContextMenu(const PName& menuName)
{
	PString menuNameStr;
	menuName.ToString(menuNameStr);

	return ImGui::BeginMenu(menuNameStr.GetCStr());
}

void PImGuiContextMenuBuild::onEndContextMenu()
{
	ImGui::EndMenu();
}

void PImGuiContextMenuBuild::onContextMenuItem(const HContextMenuTrees::HMenuData& inMenuItem)
{
	bool bVisible = inMenuItem.IsVisible.ExecuteIfBound();
	if (bVisible == false)
	{
		return;
	}

	const HMenuCommand& menuCommand = inMenuItem.MenuCommand;
	const char* label = menuCommand.MenuName.GetCStr();
	const char* shortcut = menuCommand.Shortcut.ToString().GetCStr();
	bool bEnable = inMenuItem.IsEnable.ExecuteIfBound();
	if (true == ImGui::MenuItemEx(label, 0, shortcut, false, bEnable))
	{
		inMenuItem.Action.ExecuteIfBound();
	}
}

void PImGuiContextMenuBuild::onContextMenuAction(const HContextMenuTrees::HMenuData& inMenuItem)
{
	bool bEnable = inMenuItem.IsEnable.ExecuteIfBound();
	if (bEnable == false)
	{
		return;
	}

	const HMenuCommand& menuCommand = inMenuItem.MenuCommand;
	ImGuiID widgetID = (ImGuiID)inMenuItem.OwnerGuid.GetHashCode();

	bool bPressed = isPressedShortcut(widgetID, menuCommand.Shortcut);
	if (bPressed)
	{
		inMenuItem.Action.ExecuteIfBound();
	}
}

bool PImGuiContextMenuBuild::isPressedShortcut(uint64 inWidgetID, const HMenuShortcut& inShortcut) const
{
	bool bPressed = true;

	if (inShortcut.bAlt)
	{
		bPressed &= ImGui::GetIO().KeyAlt;
	}
	if (inShortcut.bCtrl)
	{
		bPressed &= ImGui::GetIO().KeyCtrl;
	}
	if (inShortcut.bShift)
	{
		bPressed &= ImGui::GetIO().KeyShift;
	}

	for (EKeyCode keyCode : inShortcut.ShortcutKeys)
	{
		ImGuiKey imGuiKeyCode = ToImGuiKey(keyCode);
		bPressed &= ImGui::IsKeyPressed(imGuiKeyCode, (ImGuiID)inWidgetID);
	}

	if (inShortcut.ShortcutKeys.empty())
	{
		bPressed = false;
	}

	return bPressed;
}