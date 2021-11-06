#include "pch.h"
#include "GameLogicSystemLayer.h"
#include "Application.h"
#include "Class/Game/GameObject.h"

#include "Class/Game/GameNode.h"
#include "Class/Game/GameWorld.h"

#include "Class/Game/GameComponent.h"
#include "Class/Game/Components/Transform.h"
#include "Class/Game/Components/Camera.h"
#include "Class/Game/Components/SpriteRenderer.h"
#include "Class/Game/Components/StaticMeshRenderer.h"
#include "Class/Game/Components/PointLight.h"
#include "Class/Game/Components/Collision.h"
#include "Class/Game/Components/SkyDome.h"



#include "Class/Asset/Asset.h"

#include "Class/UI/ModalUI/ProgressBarModalView.h"
#include "Class/UI/ModalUI/MessageBoxModalView.h"
#include <Class/UI/UIView/InspectorView.h>
namespace JG
{
	String GameWorldPath = CombinePath(Application::GetEnginePath(), "TestGameWorld.jgasset");
	void GameLogicSystemLayer::OnAttach()
	{
		GameObjectFactory::Create();
	}
	void GameLogicSystemLayer::OnDetach()
	{
		GameObjectFactory::Destroy();
	}
	void GameLogicSystemLayer::Begin()
	{
		UIManager::GetInstance().RegisterMainMenuItem("File/Save GameWorld %_S", 0, [&](){

			if (mGameWorld == nullptr) return;


			RequestSaveGameWorldEvent e;
			Application::GetInstance().SendEvent(e);



		}, nullptr);


		RegisterGameObjectType();
		RegisterGlobalGameSystem();
		
		auto gameWorldAssetPath = ProjectSetting::GetInstance().GetStartGameWorldPath();
		if (gameWorldAssetPath.empty() == false)
		{
			RequestLoadGameWorldEvent e;
			e.AssetPath = gameWorldAssetPath;
			Application::GetInstance().SendEvent(e);
		}

		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::EndSystem, [&]() -> EScheduleResult
		{

			if (ImGui::IsKeyPressed((i32)EKeyCode::Y))
			{
				if (mGameWorld != nullptr)
				{
					DeleteGameWorld();
				}
				else
				{
					LoadGameWorld();

				}
			
			}

			return EScheduleResult::Continue;
		});
	}
	void GameLogicSystemLayer::Destroy()
	{

	}
	void GameLogicSystemLayer::OnEvent(IEvent& e)
	{
		EventDispatcher eventDispatcher(e);
		eventDispatcher.Dispatch<RequestSaveGameWorldEvent>(EVENT_BIND_FN(&GameLogicSystemLayer::ResponseSaveGameWorld));
		eventDispatcher.Dispatch<RequestLoadGameWorldEvent>(EVENT_BIND_FN(&GameLogicSystemLayer::ResponseLoadGameWorld));
		eventDispatcher.Dispatch<NotifyEditorSceneOnClickEvent>(EVENT_BIND_FN(&GameLogicSystemLayer::ResponseEditorSceneOnClick));
	}
	String GameLogicSystemLayer::GetLayerName()
	{
		return "GameLogicSystemLayer";
	}
	bool GameLogicSystemLayer::ResponseSaveGameWorld(RequestSaveGameWorldEvent& e)
	{
		if (mSaveGameWorldTaskCtrl && mSaveGameWorldTaskCtrl->IsRun())
		{
			return true;
		}
		if (mGameWorld == nullptr || mGameWorldAssetPath.empty())
		{
			return true;
		}


		{
			auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
			UIManager::GetInstance().OpenPopupUIView<ProgressBarModalView>(ProgressBarInitData("Save GameWorld"));

			progressBar->Display("Save GameWorld", 0.0f);
		}
		Scheduler::GetInstance().ScheduleOnceByFrame(0, SchedulePriority::EndSystem, [&]()->EScheduleResult
		{
			auto json = CreateSharedPtr<Json>();
			auto assetJson = json->CreateJsonData();
			mGameWorld->MakeJson(assetJson);
			json->AddMember(JG_ASSET_KEY, assetJson);



			bool result = AssetDataBase::GetInstance().WriteAsset(mGameWorldAssetPath, EAssetFormat::GameWorld, json);

			if (result == false)
			{
				auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
				UIManager::GetInstance().OpenPopupUIView< MessageBoxModalView>(MessageBoxInitData("Error", "Fail Save GameWorld"));
				progressBar->Close();
			}


			Scheduler::GetInstance().ScheduleOnce(1.0f, 0, [&]()->EScheduleResult
			{
				auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
				progressBar->Display("Save GameWorld", 1.0f);
				progressBar->Close();
				return EScheduleResult::Continue;
			});
			return EScheduleResult::Continue;
		});

		return true;
	}

	bool GameLogicSystemLayer::ResponseLoadGameWorld(RequestLoadGameWorldEvent& e)
	{
		{
			auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
			UIManager::GetInstance().OpenPopupUIView<ProgressBarModalView>(ProgressBarInitData("Load GameWorld"));

			progressBar->Display("Load GameWorld", 0.0f);
		}


		
		mGameWorldAssetPath = e.AssetPath;
		if (mGameWorld != nullptr)
		{
			GameObject::DestoryObject(mGameWorld);
			mGameWorld = nullptr;
		}
		Scheduler::GetInstance().ScheduleOnceByFrame(15, SchedulePriority::EndSystem, [&]()->EScheduleResult
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
				auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
				progressBar->Close();
				UIManager::GetInstance().OpenPopupUIView< MessageBoxModalView>(MessageBoxInitData("Error", "Fail Load GameWorld"));
			}
			mGameWorld = GameObjectFactory::GetInstance().CreateObject<GameWorld>();
			mGameWorld->SetGlobalGameSystemList(mGameSystemList);
			mGameWorld->AddFlags(EGameWorldFlags::All_Update_Lock);
			auto worldAssetJsonData = json->GetMember(JG_ASSET_KEY);
			mGameWorld->LoadJson(worldAssetJsonData);


			Scheduler::GetInstance().ScheduleOnce(1.0f, 0, [&]()->EScheduleResult
			{
				auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
				progressBar->Display("Load GameWorld", 1.0f);
				progressBar->Close();

				NotifyChangeGameWorldEvent e;
				e.GameWorld = mGameWorld;
				Application::GetInstance().SendEvent(e);

				return EScheduleResult::Continue;
			});
			return EScheduleResult::Continue;
		});

		
		return true;
	}

	bool GameLogicSystemLayer::ResponseEditorSceneOnClick(NotifyEditorSceneOnClickEvent& e)
	{
		NotifySelectedGameNodeInEditorEvent pickingEvent;
		if (mGameWorld != nullptr)
		{
			List<IJGObject*> exceptObjectList;
			auto inspectorView = UIManager::GetInstance().GetUIView<InspectorView>();
			if (inspectorView)
			{
				exceptObjectList.push_back(inspectorView->GetTargetObject());
			}


			pickingEvent.SelectedGameNode = mGameWorld->Picking(e.ClickPos, exceptObjectList);
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
		GameObjectFactory::GetInstance().RegisterComponentType<PointLight>();
		GameObjectFactory::GetInstance().RegisterComponentType<SkyDome>();

	}
	void GameLogicSystemLayer::LoadGameWorld()
	{
		JGGraphics::GetInstance().Flush();
		mGameWorld = GameObjectFactory::GetInstance().CreateObject<GameWorld>();
		mGameWorld->SetGlobalGameSystemList(mGameSystemList);
		mGameWorld->AddFlags(EGameWorldFlags::All_Update_Lock);

		auto worldAssetJson = CreateSharedPtr<Json>();
		AssetDataBase::GetInstance().ReadAsset(GameWorldPath, nullptr, &(worldAssetJson));
		auto worldAssetJsonData = worldAssetJson->GetMember(JG_ASSET_KEY);
		mGameWorld->LoadJson(worldAssetJsonData);

		{
			NotifyChangeGameWorldEvent e;
			e.GameWorld = mGameWorld;
			Application::GetInstance().SendEvent(e);
		}
	}
	void GameLogicSystemLayer::DeleteGameWorld()
	{
		GameObject::DestoryObject(mGameWorld);
		mGameWorld = nullptr;
	}
}