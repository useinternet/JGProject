#pragma once
#include "JGCore.h"
#include "Class/Data/Shared/EventData.h"

namespace JG
{
	REQUEST_ONE_ARG_EVENT(RequestOpenProgressBarEvent, Shared::Editor::ProgressBarOpenData, OpenData, {})
	REQUEST_ONE_ARG_EVENT(RequestDisplayProgressBarEvent, Shared::Editor::ProgressBarDisplayData, DisplayData, {})
	REQUEST_EVENT(RequestCloseProgressBarEvent)


	REQUEST_ONE_ARG_EVENT(RequestOpenMessageBoxEvent, Shared::Editor::MessageBoxOpenData, OpenData, {})




	NOTIFY_ONE_ARG_EVENT(NotifySelectedGameNodeInEditorEvent, class GameNode*, SelectedGameNode, nullptr)
	NOTIFY_ONE_ARG_EVENT(NotifySelectedAssetInEditorEvent, String, SelectedAssetPath, "")


	NOTIFY_TWO_ARG_EVENT(NotifyEditorSceneOnClickEvent, JVector2, ClickPos, JVector2(0, 0), List<IJGObject*>, ExceptObjectList, {})
}