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
		

		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::Graphics_DestroyObject + 1, [&]() -> EScheduleResult
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
		if (mGameWorld == nullptr)
		{
			return true;
		}
		enum
		{
			Task_Seq_WriteJson,
			Task_Seq_SaveJson,
		};
		

		auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
		UIManager::GetInstance().OpenPopupUIView<ProgressBarModalView>(ProgressBarInitData("Save GameWorld"));
		mSaveGameWorldTaskCtrl = CreateUniquePtr<TaskController>();
		mSaveGameWorldTaskCtrl->SetPriority(SchedulePriority::EndSystem);
		mSaveGameWorldTaskCtrl->SetStartID(Task_Seq_WriteJson);
		mSaveGameWorldTaskCtrl->SetUserData(CreateSharedPtr<SaveGameWorldData>());

		Scheduler::GetInstance().ScheduleOnce(0.15f, 0, [&]() -> EScheduleResult
		{
			mSaveGameWorldTaskCtrl->AddTask(Task_Seq_WriteJson, nullptr,
				[&](SharedPtr<IJGObject> userData)
			{
				auto data = userData->As<SaveGameWorldData>();
				auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
				progressBar->Display("Write Json...", 0.25f);
				data->GameWorld = mGameWorld;
				data->Json = CreateSharedPtr<Json>();
				data->Path = GameWorldPath;


			}, [&](SharedPtr<IJGObject> userData) -> i32
			{
				auto data = userData->As<SaveGameWorldData>();


				auto assetJson = data->Json->CreateJsonData();
				data->GameWorld->MakeJson(assetJson);
				data->Json->AddMember(JG_ASSET_KEY, assetJson);


			

				return Task_Seq_SaveJson;
			});


			mSaveGameWorldTaskCtrl->AddTask(Task_Seq_SaveJson, nullptr,
				[&](SharedPtr<IJGObject> userData)
			{

				auto data = userData->As<SaveGameWorldData>();
				auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
				progressBar->Display("Save Json...", 0.75f);

				data->IsSuccessed = AssetDataBase::GetInstance().WriteAsset(data->Path, EAssetFormat::GameWorld, data->Json);

			}, [&](SharedPtr<IJGObject> userData) -> i32
			{
				auto data = userData->As<SaveGameWorldData>();
				auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
				progressBar->Display("Save Compelete", 1.0f);

				Scheduler::GetInstance().ScheduleOnce(1.0f, 0, []()->EScheduleResult
				{
					auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
					progressBar->Close();
					return EScheduleResult::Break;
				});
			

				if (data->IsSuccessed == false)
				{
					UIManager::GetInstance().OpenPopupUIView< MessageBoxModalView>(MessageBoxInitData("Error", "Fail Save GameWorld"));
				}
				return -1;
			});
			mSaveGameWorldTaskCtrl->Run();

			return EScheduleResult::Break;
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