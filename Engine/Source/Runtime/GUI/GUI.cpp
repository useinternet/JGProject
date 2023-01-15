#include "PCH/PCH.h"
#include "GUI.h"
#include "Builder/GUIBuilder.h"
#include "imgui/imgui.h"

void GGUIGlobalSystem::Start()
{
	_memObject = Allocate<IMemoryObject>();
	GScheduleGlobalSystem::GetInstance().ScheduleByFrame(_memObject, EMainThreadExecutionOrder::Update, SCHEDULE_FN_BIND(GGUIGlobalSystem::BuildGUI));
}

void GGUIGlobalSystem::Destroy()
{
	_memObject = nullptr;
}

void GGUIGlobalSystem::BuildGUI(const PTaskArguments& args)
{
	HGUIBuilder guiBuilder;

	for (const HPair<HGuid, PSharedPtr<WWidget>>& pair : _openWidgets)
	{
		PSharedPtr<WWidget> widget = pair.second;

		guiBuilder.BeginWidget(widget);

		widget->OnGUIBuild(guiBuilder);

		guiBuilder.EndWidget();
	}

	guiBuilder.Build();
}
