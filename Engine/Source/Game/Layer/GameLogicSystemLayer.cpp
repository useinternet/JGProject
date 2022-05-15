#include "pch.h"
#include "GameLogicSystemLayer.h"
#include "Application.h"
#include "Class/Data/StorableVar.h"
#include "GameObject.h"
#include "GameNode.h"
#include "GameWorld.h"
#include "GameComponent.h"
#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Components/SpriteRenderer.h"
#include "Components/StaticMeshRenderer.h"
#include "Components/SkeletalMeshRenderer.h"
#include "Components/PointLight.h"
#include "Components/DirectionalLight.h"
#include "Components/SpotLight.h"
#include "Components/Collision.h"
#include "Components/SkyDome.h"
#include "Components/DevComponent.h"
#include "Manager/GameLayerManager.h"
#include "Class/Asset/Asset.h"


#include "Graphics/DebugGeometryDrawer.h"
#include "Graphics/Resource.h"
#define GAME_DLL_NAME "SandBox_Game.dll"


#include "Class/Asset\AssetImporter.h"
namespace JG
{
	void GameLogicSystemLayer::OnAttach()
	{
		GlobalSingletonManager::GetInstance().RegisterSingleton<GameLayerManager>();
		GlobalSingletonManager::GetInstance().RegisterSingleton<GameObjectFactory>();
	}
	void GameLogicSystemLayer::OnDetach()
	{
		GlobalSingletonManager::GetInstance().UnRegisterSingleton<GameLayerManager>();
		GlobalSingletonManager::GetInstance().UnRegisterSingleton<GameObjectFactory>();
	}
	void GameLogicSystemLayer::Begin()
	{
		mGamePlugin = CreateUniquePtr<Plugin>(GAME_DLL_NAME);
		mGamePlugin->Link(CreateSharedPtr<PluginLinker>());


		RegisterGameObjectType();
		RegisterGlobalGameSystem();

		auto gameWorldAssetPath = ProjectSetting::GetInstance().GetStartGameWorldPath();
		if (gameWorldAssetPath.empty() == false)
		{
			RequestLoadGameWorldEvent e;
			e.AssetPath = gameWorldAssetPath;
			Application::GetInstance().SendEvent(e);
		}
	}
	void GameLogicSystemLayer::Destroy()
	{

	}
	void GameLogicSystemLayer::OnEvent(IEvent& e)
	{
		EventDispatcher eventDispatcher(e);
		eventDispatcher.Dispatch<RequestSaveGameWorldEvent>(EVENT_BIND_FN(&GameLogicSystemLayer::ResponseSaveGameWorld));
		eventDispatcher.Dispatch<RequestLoadGameWorldEvent>(EVENT_BIND_FN(&GameLogicSystemLayer::ResponseLoadGameWorld));

		eventDispatcher.Dispatch<RequestPlayGameEvent>(EVENT_BIND_FN(&GameLogicSystemLayer::ResponsePlayGame));
		eventDispatcher.Dispatch<RequestPauseGameEvent>(EVENT_BIND_FN(&GameLogicSystemLayer::ResponsePauseGame));
		eventDispatcher.Dispatch<RequestStopGameEvent>(EVENT_BIND_FN(&GameLogicSystemLayer::ResponseStopGame));

		eventDispatcher.Dispatch<NotifyEditorSceneOnClickEvent>(EVENT_BIND_FN(&GameLogicSystemLayer::ResponseEditorSceneOnClick));
	}
	String GameLogicSystemLayer::GetLayerName()
	{
		return "GameLogicSystemLayer";
	}
	bool GameLogicSystemLayer::ResponseSaveGameWorld(RequestSaveGameWorldEvent& e)
	{
		if (mGameWorld == nullptr || mGameWorldAssetPath.empty() && mIsGamePlaying == true)
		{
			return true;
		}


		{
			RequestOpenProgressBarEvent e;
			e.OpenData.Title    = "Save GameWorld";
			e.OpenData.Contents = "Save GameWorld...";
			e.OpenData.Ratio    = 0.5f;
			Application::GetInstance().SendEvent(e);
		}


		Scheduler::GetInstance().ScheduleOnceByFrame(0, SchedulePriority::EndSystem,
			[&](SharedPtr<IJGObject> userData)->EScheduleResult
		{
			auto json = CreateSharedPtr<Json>();
			auto assetJson = json->CreateJsonData();
			mGameWorld->MakeJson(assetJson);
			json->AddMember(JG_ASSET_KEY, assetJson);



			bool result = AssetDataBase::GetInstance().WriteAsset(mGameWorldAssetPath, EAssetFormat::GameWorld, json);
			JG_LOG_INFO("Compelete Save GameWorld : {0} ", mGameWorldAssetPath);
			if (result == false)
			{
				{
					RequestCloseProgressBarEvent e;
					Application::GetInstance().SendEvent(e);
				}
				{
					RequestOpenMessageBoxEvent e;
					e.OpenData.Title    = "Error";
					e.OpenData.Contents = "Fail Save GameWorld";
					Application::GetInstance().SendEvent(e);
				}
			}


			Scheduler::GetInstance().ScheduleOnce(1.0f, 0, [&](SharedPtr<IJGObject> userData)->EScheduleResult
			{
				{
					RequestCloseProgressBarEvent e;
					Application::GetInstance().SendEvent(e);
				}
				ActionData* actionData = userData->As<ActionData>();
				if (actionData && actionData->CompeleteAction != nullptr)
				{
					actionData->CompeleteAction();
				}
				return EScheduleResult::Continue;
			}, CreateSharedPtr<ActionData>(userData->As<ActionData>()->CompeleteAction));

			return EScheduleResult::Continue;
		}, CreateSharedPtr<ActionData>(e.CompeleteAction));

		return true;
	}
	bool GameLogicSystemLayer::ResponseLoadGameWorld(RequestLoadGameWorldEvent& e)
	{
		{
			RequestOpenProgressBarEvent e;
			e.OpenData.Title = "Load GameWorld";
			e.OpenData.Contents = "Load GameWorld...";
			e.OpenData.Ratio = 0.5f;
			Application::GetInstance().SendEvent(e);
		}


		
		mGameWorldAssetPath = e.AssetPath;
		if (mGameWorld != nullptr)
		{
			GameObject::DestoryObject(mGameWorld);
			mGameWorld = nullptr;
		}
		Scheduler::GetInstance().ScheduleOnceByFrame(15, SchedulePriority::EndSystem, [&](SharedPtr<IJGObject> userData)->EScheduleResult
		{
			auto json = CreateSharedPtr<Json>();
			EAssetFormat assetFormat;
			bool result = AssetDataBase::GetInstance().ReadAsset(mGameWorldAssetPath, &assetFormat, &json);
			if (assetFormat != EAssetFormat::GameWorld)
			{
				result = false;
			}


			if (result == false)
			{
				{
					RequestCloseProgressBarEvent e;
					Application::GetInstance().SendEvent(e);
				}
				{
					RequestOpenMessageBoxEvent e;
					e.OpenData.Title = "Error";
					e.OpenData.Contents = "Fail Load GameWorld";
					Application::GetInstance().SendEvent(e);
				}
			}
			mGameWorld = GameObjectFactory::GetInstance().CreateObject<GameWorld>();
			mGameWorld->SetGlobalGameSystemList(mGameSystemList);
			mGameWorld->AddFlags(EGameWorldFlags::All_Update_Lock);
			auto worldAssetJsonData = json->GetMember(JG_ASSET_KEY);
			mGameWorld->LoadJson(worldAssetJsonData);


			Scheduler::GetInstance().ScheduleOnce(1.0f, 0, [&](SharedPtr<IJGObject> userData)->EScheduleResult
			{
				{
					RequestCloseProgressBarEvent e;
					Application::GetInstance().SendEvent(e);
				}
				ActionData* actionData = userData->As<ActionData>();
				if (actionData && actionData->CompeleteAction != nullptr)
				{
					actionData->CompeleteAction();
				}


				{
					NotifyChangeGameWorldEvent e;
					e.GameWorld = mGameWorld;
					Application::GetInstance().SendEvent(e);
				}

				return EScheduleResult::Continue;
			}, CreateSharedPtr<ActionData>(userData->As<ActionData>()->CompeleteAction));
			return EScheduleResult::Continue;
		}, CreateSharedPtr<ActionData>(e.CompeleteAction));

		
		return true;
	}
	bool GameLogicSystemLayer::ResponsePlayGame(RequestPlayGameEvent& e)
	{
		if (mGameWorld == nullptr)
		{
			return true;
		}
		if (mIsGamePlaying == false)
		{
			RequestSaveGameWorldEvent e;
			e.CompeleteAction = [&]()
			{
				Camera* editorCam = Camera::GetMainCamera();
				if (editorCam->GetType() == JGTYPE(EditorCamera))
				{
					editorCam->GetOwner()->Destroy(editorCam);
				}
				mGameWorld->RemoveFlags(EGameWorldFlags::All_Update_Lock);
				mIsGamePlaying = true;
			};
			Application::GetInstance().SendEvent(e);
		}
		else
		{
			mGameWorld->RemoveFlags(EGameWorldFlags::All_Update_Lock);
		}

		return true;
	}
	bool GameLogicSystemLayer::ResponsePauseGame(RequestPauseGameEvent& e)
	{
		if (mGameWorld == nullptr)
		{
			return true;
		}
		mGameWorld->AddFlags(EGameWorldFlags::All_Update_Lock);
		return true;
	}
	bool GameLogicSystemLayer::ResponseStopGame(RequestStopGameEvent& e)
	{
		if (mGameWorld == nullptr)
		{
			return true;
		}

		if (mIsGamePlaying == true)
		{
			RequestLoadGameWorldEvent e;
			e.AssetPath = mGameWorldAssetPath;
			e.CompeleteAction = [&]()
			{
				mIsGamePlaying = false;
			};
			Application::GetInstance().SendEvent(e);
		}
		return true;
	}
	bool GameLogicSystemLayer::ResponseEditorSceneOnClick(NotifyEditorSceneOnClickEvent& e)
	{
		NotifySelectedGameNodeInEditorEvent pickingEvent;
		if (mGameWorld != nullptr)
		{
			pickingEvent.SelectedGameNode = mGameWorld->Picking(e.ClickPos, e.ExceptObjectList);
		}
		else
		{
			pickingEvent.SelectedGameNode = nullptr;
		}
		Application::GetInstance().SendEvent(pickingEvent);
		return true;
	}
	void GameLogicSystemLayer::RegisterGlobalGameSystem()
	{



	}
	void GameLogicSystemLayer::RegisterGameObjectType()
	{
		// GameNode
		GameObjectFactory::GetInstance().RegisterNodeType<GameNode>();
		GameObjectFactory::GetInstance().RegisterNodeType<GameWorld>();

		// GameComponent
		GameObjectFactory::GetInstance().RegisterComponentType<Transform>();
		GameObjectFactory::GetInstance().RegisterComponentType<Camera>();
		GameObjectFactory::GetInstance().RegisterComponentType<EditorCamera>();
		GameObjectFactory::GetInstance().RegisterComponentType<SpriteRenderer>();
		GameObjectFactory::GetInstance().RegisterComponentType<StaticMeshRenderer>();
		GameObjectFactory::GetInstance().RegisterComponentType<SkeletalMeshRenderer>();
		GameObjectFactory::GetInstance().RegisterComponentType<PointLight>();
		GameObjectFactory::GetInstance().RegisterComponentType<DirectionalLight>();
		GameObjectFactory::GetInstance().RegisterComponentType<SpotLight>();
		GameObjectFactory::GetInstance().RegisterComponentType<SkyDome>();
		GameObjectFactory::GetInstance().RegisterComponentType<DevComponent>();

		auto t = mGamePlugin->LoadFunction<void>("DevPlayer_Register_GameComponent");
		// GamePlugin
		//
		StorableString  componentTypeCache	 = StorableString("GameLogicSystemLayer/GameComponentTypeCache", "");
		HashSet<String> cachedComponentTypeSet;

		std::stringstream ss(componentTypeCache.GetValue());
		String tmp;
		while (std::getline(ss, tmp, ',')) {
			cachedComponentTypeSet.insert(tmp);
		}
		const String& cppPath = Application::GetCppPath();
		fs::recursive_directory_iterator dirIter = fs::recursive_directory_iterator(cppPath);

		for (auto iter : dirIter)
		{
			const fs::path& p = iter.path();
			String extension = p.extension().string();
			if (extension != ".h")
			{
				continue;
			}
			String comTypeName = StringHelper::ReplaceAll(p.filename().string(), p.extension().string(), "");
			if (cachedComponentTypeSet.find(comTypeName) != cachedComponentTypeSet.end())
			{
				continue;
			}
			cachedComponentTypeSet.insert(comTypeName);
		}

		String componentTypeCacheStr;
		for (const String& typeName : cachedComponentTypeSet)
		{
			String gameComponentFuncName = typeName + "_Register_GameComponent";
			PluginFunction<void> gameComponentFunc = mGamePlugin->LoadFunction<void>(gameComponentFuncName);

			if (gameComponentFunc.IsVaild() == true)
			{
				gameComponentFunc();
				componentTypeCacheStr += typeName + ",";
			}
		}
		componentTypeCache.SetValue(componentTypeCacheStr);
	}
}