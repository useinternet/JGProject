#include "PCH/PCH.h"
#include "ImGuiMenuBuild.h"
#include "Widget.h"
#include "ImGuiBuildUtility.h"

 void PImGuiMenuBuild::PushData(const HMenuTrees& inMenuTrees)
{
	 _menuTrees = inMenuTrees;
	 bTreeDirty = true;
}

void PImGuiMenuBuild::Build()
{
	if (bTreeDirty)
	{
		HMenuTrees::HEvents bindEvents;
		bindEvents.OnBeginMainMenu	= HMenuTrees::HOnBeginMainMenu::CreateSP(SharedWrap(this), &PImGuiMenuBuild::onBeginMainMenu);
		bindEvents.OnEndMainMenu	= HMenuTrees::HOnEndMainMenu::CreateSP(SharedWrap(this), &PImGuiMenuBuild::onEndMainMenu);
		bindEvents.OnMainMenuItem	= HMenuTrees::HOnMainMenuItem::CreateSP(SharedWrap(this), &PImGuiMenuBuild::onMainMenuItem);
		bindEvents.OnMainMenuAction = HMenuTrees::HOnMainMenuAction::CreateSP(SharedWrap(this), &PImGuiMenuBuild::onMainMenuAction);
		bindEvents.OnMenuAction		= HMenuTrees::HOnMenuAction::CreateSP(SharedWrap(this), &PImGuiMenuBuild::onMenuAction);

		_menuTrees.BindEvents(bindEvents);
		bTreeDirty = false;
	}
	if (ImGui::BeginMainMenuBar())
	{
		_menuTrees.UpdateTrees();

		ImGui::EndMainMenuBar();
	}
}

void PImGuiMenuBuild::Reset()
{

}

bool PImGuiMenuBuild::onBeginMainMenu(const PName& menuName)
{
	PString menuNameStr;
	menuName.ToString(menuNameStr);

	return ImGui::BeginMenu(menuNameStr.GetCStr());
}

void PImGuiMenuBuild::onEndMainMenu()
{
	ImGui::EndMenu();
}

void PImGuiMenuBuild::onMainMenuItem(const HMenuTrees::HMainMenuData& inMainMenuItem)
{
	bool bVisible = inMainMenuItem.IsVisible.ExecuteIfBound();
	if (bVisible == false)
	{
		return;
	}

	const HMenuCommand& menuCommand = inMainMenuItem.MenuCommand;
	const char* label = menuCommand.MenuName.GetCStr();
	const char* shortcut = menuCommand.Shortcut.ToString().GetCStr();
	bool bEnable = inMainMenuItem.IsEnable.ExecuteIfBound();
	if (true == ImGui::MenuItemEx(label, 0, shortcut, false, bEnable))
	{
		inMainMenuItem.Action.ExecuteIfBound();
	}
}

void PImGuiMenuBuild::onMainMenuAction(const HMenuTrees::HMainMenuData& inMainMenuItem)
{
	bool bEnable = inMainMenuItem.IsEnable.ExecuteIfBound();
	if (bEnable == false)
	{
		return;
	}

	const HMenuCommand& menuCommand = inMainMenuItem.MenuCommand;
	ImGuiID mainWidgetID = ImHashStr(HGUIConstants::MainWidgetID.GetCStr());

	bool bPressed = isPressedShortcut(mainWidgetID, menuCommand.Shortcut);
	if (bPressed)
	{
		inMainMenuItem.Action.ExecuteIfBound();
	}
}

void PImGuiMenuBuild::onMenuAction(const HMenuTrees::HMenuData& inMenuItem)
{
	PSharedPtr<WWidget> ownerWidget = inMenuItem.Widget.Pin();
	if (ownerWidget.IsValid())
	{
		return;
	}

	const HMenuCommand& menuCommand = inMenuItem.MenuCommand;
	uint64 ownerWidgetID = ownerWidget->GetGuid().GetHashCode();

	bool bPressed = isPressedShortcut(ownerWidgetID, menuCommand.Shortcut);
	if (bPressed)
	{
		inMenuItem.Action.ExecuteIfBound();
	}
}

bool PImGuiMenuBuild::isPressedShortcut(uint64 inWidgetID, const HMenuShortcut& inShortcut) const
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
