#include "PCH/PCH.h"
#include "GUI.h"
#include "Builder/GUIBuilder.h"
#include "Builder/ContextMenuBuilder.h"
#include "Builder/MenuBuilder.h"
#include "GUIStyles/GUIStyle.h"
#include "Datas/GUIData.h"


const PString HGUIConstants::MainWidgetID = "DEFAULT_UI_SYSTEM_LAYER";

void GGUIGlobalSystem::Start()
{
	_memObject  = Allocate<IMemoryObject>();
	_bMenuDirty = true;

	GScheduleGlobalSystem::GetInstance().ScheduleByFrame(EMainThreadExecutionOrder::Update, PTaskDelegate::CreateRaw(this, &GGUIGlobalSystem::Update));
	GScheduleGlobalSystem::GetInstance().ScheduleByFrame(EMainThreadExecutionOrder::Update, PTaskDelegate::CreateRaw(this, &GGUIGlobalSystem::Build));
	CollectGUIStyles();
	LoadGUIDatas();
}

void GGUIGlobalSystem::Destroy()
{
	SaveGUIDatas();
	_memObject = nullptr;
}

void GGUIGlobalSystem::Update()
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

void GGUIGlobalSystem::Build()
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

void GGUIGlobalSystem::SaveGUIDatas()
{
	static PString savePath = HFileHelper::EngineTempDirectory() / "jg_gui.json";

	JGGUIData guiData;
	for (const HPair<HGuid, PSharedPtr<WWidget>>& pair : _widgetPool)
	{
		HGuid				widgetGuid  = pair.first;
		PSharedPtr<WWidget> widget		= pair.second;
		if (widget.IsValid() == false)
		{
			continue;
		}

		if (_widgetNameRedirectByGuid.contains(widgetGuid))
		{
			PName widgetKeyName = _widgetNameRedirectByGuid[widgetGuid];
			guiData.RedirectMap.emplace(widgetGuid, widgetKeyName);
		}

		guiData.Widgets.push_back(widget);
	}

	SaveObject(savePath, &guiData);
}

void GGUIGlobalSystem::LoadGUIDatas()
{
	static PString loadPath = HFileHelper::EngineTempDirectory() / "jg_gui.json";

	PSharedPtr<JGGUIData> guiData = LoadObject<JGGUIData>(loadPath);
	if (guiData == nullptr)
	{
		return;
	}

	for (PSharedPtr<WWidget> widget : guiData->Widgets)
	{
		if (widget.IsValid() == false)
		{
			continue;
		}

		HGuid widgetGuid = widget->GetGuid();

		_widgetPool.emplace(widgetGuid, widget);
		if (guiData->RedirectMap.contains(widgetGuid))
		{
			PName widgetKeyName = guiData->RedirectMap[widgetGuid];
			_widgetGuidRedirectByName.emplace(widgetKeyName, widgetGuid);
			_widgetNameRedirectByGuid.emplace(widgetGuid, widgetKeyName);
		}
		else
		{
			_widgetGuidRedirectByType.emplace(widget->GetType(), widgetGuid);
		}

		if (widget->_bIsOpen)
		{
			_openWidgets.emplace(widgetGuid, widget);
			widget->OnOpen();
		}
	}
}

void GGUIGlobalSystem::CollectGUIStyles()
{
	JG_LOG(GUI, ELogLevel::Info, "Start Collect GUI Styles...");

	PSharedPtr<JGClass>		   GUIStyleClass   = StaticClass<JGGUIStyle>();
	HList<PSharedPtr<JGClass>> GUIStyleClasses = GUIStyleClass->GetChildClasses(true);

	for (PSharedPtr<JGClass> Class : GUIStyleClasses)
	{
		PSharedPtr<JGType> ClassType = Class->GetClassType();
		
		_guiStylePool.emplace(*ClassType, AllocateByClass<JGGUIStyle>(Class));

		JG_LOG(GUI, ELogLevel::Debug, "- %s ", Class->GetClassType()->GetName().ToString());
	}

	JG_LOG(GUI, ELogLevel::Info, "End Collect GUI Styles...");
}
