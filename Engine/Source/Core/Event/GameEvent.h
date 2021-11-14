#pragma once
#include "JGCore.h"
#include "Class/Data/Shared/EventData.h"
namespace JG
{
	NOTIFY_ONE_ARG_EVENT(NotifyGameComponentTypeSet, HashSet<Type>, GameComponentTypeList, HashSet<Type>())
	NOTIFY_ONE_ARG_EVENT(NotifyDestroyJGObjectEvent, class IJGObject*, DestroyedObject, nullptr)
	NOTIFY_ONE_ARG_EVENT(NotifyChangeMainSceneTextureEvent, SharedPtr<class ITexture>, SceneTexture, nullptr)
	NOTIFY_ONE_ARG_EVENT(NotifyChangeGameWorldEvent, class GameWorld*, GameWorld, nullptr)


	REQUEST_ONE_ARG_EVENT(RequestPlayGameEvent, std::function<void()>, CompeleteAction, nullptr)
	REQUEST_ONE_ARG_EVENT(RequestStopGameEvent, std::function<void()>, CompeleteAction, nullptr)
	REQUEST_ONE_ARG_EVENT(RequestPauseGameEvent, std::function<void()>, CompeleteAction, nullptr)
	REQUEST_ONE_ARG_EVENT(RequestSaveGameWorldEvent, std::function<void()>, CompeleteAction, nullptr)
	REQUEST_TWO_ARG_EVENT(RequestLoadGameWorldEvent, String, AssetPath, "", std::function<void()>, CompeleteAction, nullptr)
}