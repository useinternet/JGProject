#pragma once
#include "Core.h"
#include "Builder/GUIBuilder.h"

JG_DECLARE_DELEGATE(HOnGUIContent, HGUIBuilder&)
JG_DECLARE_MULTICAST_DELEGATE(HOnPreGUIContent, HGUIBuilder&)
JG_DECLARE_MULTICAST_DELEGATE(HOnPostGUIContent, HGUIBuilder&)
class HWidgetContext
{
public:
	HVector2 ContentSize;

	mutable HOnPreGUIContent  OnPrePreContent;
	mutable HOnPostGUIContent OnPostGUIContent;

public:
	void PushContent(const HOnGUIContent& InContent) const
	{
		HGUIBuilder guiBuilder;

		OnPrePreContent.BroadCast(guiBuilder);

		InContent.ExecuteIfBound(guiBuilder);

		OnPostGUIContent.BroadCast(guiBuilder);
	}
};