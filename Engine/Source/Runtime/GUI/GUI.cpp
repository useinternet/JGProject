#include "PCH/PCH.h"
#include "GUI.h"
#include "Builder/GUIBuilder.h"
#include "Builder/ContextMenuBuilder.h"
#include "Builder/MenuBuilder.h"


const PString HGUIConstants::MainWidgetID = "DEFAULT_UI_SYSTEM_LAYER";

void GGUIGlobalSystem::Start()
{
	_memObject  = Allocate<IMemoryObject>();
	_bMenuDirty = true;

	GScheduleGlobalSystem::GetInstance().ScheduleByFrame(_memObject, EMainThreadExecutionOrder::Update, SCHEDULE_FN_BIND(GGUIGlobalSystem::Update));
	GScheduleGlobalSystem::GetInstance().ScheduleByFrame(_memObject, EMainThreadExecutionOrder::Update, SCHEDULE_FN_BIND(GGUIGlobalSystem::Build));
}

void GGUIGlobalSystem::Destroy()
{
	_memObject = nullptr;
}

void GGUIGlobalSystem::Update(const PTaskArguments& args)
{
	for (const HPair<HGuid, PSharedPtr<WWidget>>& pair : _openWidgets)
	{
		PSharedPtr<WWidget> widget = pair.second;
		if (widget == nullptr || widget->_bAllowUpdate == false)
		{
			continue;
		}

		widget->OnUpdate();
	}
}

void GGUIGlobalSystem::Build(const PTaskArguments& args)
{
	BuildGUI();

	BuildMenu();
	BuildContextMenu();

	if (_menuBuild.IsValid())
	{
		_menuBuild->Build();
		_menuBuild->Reset();
	}

	if (_guiBuild.IsValid())
	{
		_guiBuild->Build();
		_guiBuild->Reset();
	}
}

void GGUIGlobalSystem::BuildGUI()
{
	HGUIBuilder guiBuilder;

	for (const HPair<HGuid, PSharedPtr<WWidget>>& pair : _openWidgets)
	{
		PSharedPtr<WWidget> widget = pair.second;

		guiBuilder.BeginWidget(widget);

		widget->OnGUIBuild(guiBuilder);

		guiBuilder.EndWidget();
	}

	if (_guiBuild.IsValid())
	{
		_guiBuild->PushData(guiBuilder.GetCommandQueue());
	}
}

void GGUIGlobalSystem::BuildMenu()
{
	HMenuBuilder menuBuilder;
	GUIEvents.OnMenuBuild.BroadCast(menuBuilder);

	for (const HPair<HGuid, PSharedPtr<WWidget>>& pair : _openWidgets)
	{
		PSharedPtr<WWidget> widget = pair.second;
		widget->OnMenuBuild(menuBuilder);
	}

	if (_menuBuild.IsValid())
	{
		_menuBuild->PushData(menuBuilder.GetMenuTrees());
	}
}

void GGUIGlobalSystem::BuildContextMenu()
{
	HContextMenuBuilder contextMenuBuilder;
	GUIEvents.OnContextMenuBuild.BroadCast(contextMenuBuilder);

	for (const HPair<HGuid, PSharedPtr<WWidget>>& pair : _openWidgets)
	{
		PSharedPtr<WWidget> widget = pair.second;
		widget->OnContextMenuBuild(contextMenuBuilder);
	}

}

void GGUIGlobalSystem::SetGUIBuild(PSharedPtr<IGUIBuild> guiBuild)
{
	_guiBuild = guiBuild;
}

PSharedPtr<IGUIBuild> GGUIGlobalSystem::GetGUIBuild() const
{
	return _guiBuild;
}

void GGUIGlobalSystem::SetMenuBuild(PSharedPtr<IMenuBuild> inMenuBuild)
{
	_menuBuild = inMenuBuild;
}

PSharedPtr<IMenuBuild> GGUIGlobalSystem::GetMenuBuild() const
{
	return _menuBuild;
}

void GGUIGlobalSystem::SetContextMenuBuild(PSharedPtr<IContextMenuBuild> inContextMenuBuild)
{
	_contextMenuBuild = inContextMenuBuild;
}

PSharedPtr<IContextMenuBuild> GGUIGlobalSystem::GetContextMenuBuild() const
{
	return _contextMenuBuild;
}
