#pragma once
#include "Core.h"
#include "Builder/GUIBuilder.h"

JG_DECLARE_DELEGATE_ONEPARAM(HOnGUIContent, HGUIBuilder&)
JG_DECLARE_MULTI_DELEGATE_ONEPARAM(HOnPreGUIContent, HGUIBuilder&)
JG_DECLARE_MULTI_DELEGATE_ONEPARAM(HOnPostGUIContent, HGUIBuilder&)
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