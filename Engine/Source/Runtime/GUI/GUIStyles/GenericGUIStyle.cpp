#include "PCH/PCH.h"
#include "GenericGUIStyle.h"
#include "GUI.h"

void JGGenericGUIStyle::Construct()
{
	SetStyleColor(EStyleColor::Button		, HLinearColor(0.26f, 0.59f, 0.98f, 0.40f));
	SetStyleColor(EStyleColor::ButtonActive	, HLinearColor(0.06f, 0.53f, 0.98f, 1.00f));
	SetStyleColor(EStyleColor::ButtonHovered, HLinearColor(0.26f, 0.59f, 0.98f, 1.00f));
	SetStyleColor(EStyleColor::Border		, HLinearColor(0.06f, 0.06f, 0.06f, 0.4f));
	SetStyleColor(EStyleColor::BorderActive	, HLinearColor(0.1f, 0.1f, 0.1f, 0.98f));
	SetStyleColor(EStyleColor::BorderHovered, HLinearColor(0.12f, 0.12f, 0.12f, 0.4f));
	SetStyleColor(EStyleColor::BorderDisable, HLinearColor(0.10f, 0.10f, 0.10f, 0.54f));
}
JGGenericGUIStyle& JGGenericGUIStyle::Get()
{
	return GGUIGlobalSystem::GetInstance().GetGUIStyle<JGGenericGUIStyle>();
}
