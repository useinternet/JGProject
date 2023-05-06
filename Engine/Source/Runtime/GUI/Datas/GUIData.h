#pragma once
#include "Core.h"
#include "WidgetData.h"
#include "Widget.h"
#include "Runtime/CodeGen/Module.GUI.GUIData.generation.h"


JGCLASS()
class JGGUIData : public JGObject
{
	JG_GENERATED_CLASS_BODY
public:

	JGPROPERTY()
	HList<PSharedPtr<WWidget>> Widgets;

	JGPROPERTY()
	HHashMap<HGuid,PName> RedirectMap;
};
