#include "PCH/PCH.h"
#include "GUI.h"
#include "Builder/GUIBuilder.h"

void GGUIGlobalSystem::Start()
{
	_memObject = Allocate<IMemoryObject>();
	GScheduleGlobalSystem::GetInstance().ScheduleByFrame(_memObject, EMainThreadExecutionOrder::Update, SCHEDULE_FN_BIND(GGUIGlobalSystem::Build));
}

void GGUIGlobalSystem::Destroy()
{
	_memObject = nullptr;
}

void GGUIGlobalSystem::Build(const PTaskArguments& args)
{
	BuildGUI();
	BuildMenu();
	BuildContextMenu();

	_guiBuild->Build();
	_guiBuild->Reset();
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

	_guiBuild->PushData(guiBuilder.GetCommandQueue());
}

void GGUIGlobalSystem::BuildMenu()
{
}

void GGUIGlobalSystem::BuildContextMenu()
{
}

void GGUIGlobalSystem::SetGUIBuild(PSharedPtr<IGUIBuild> guiBuild)
{
	_guiBuild = guiBuild;
}

PSharedPtr<IGUIBuild> GGUIGlobalSystem::GetGUIBuild() const
{
	return _guiBuild;
}
