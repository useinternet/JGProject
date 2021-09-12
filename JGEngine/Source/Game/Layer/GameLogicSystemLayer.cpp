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
#include "Class/Asset/Asset.h"

#include "Class/UI/ModalUI/ProgressBarModalView.h"
#include <Class/UI/UIView/InspectorView.h>
namespace JG
{
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
		//static bool testopen = false;
		//UIManager::GetInstance().RegisterMainMenuItem("File/Save World %_S", 0, [&]() {
		//	SaveGameWorld();
		//}, nullptr);

		//UIManager::GetInstance().RegisterMainMenuItem("File/Load World", 0, [&]() {
		//
		//	LoadGameWrold();
		//}, nullptr);

		RegisterGameObjectType();
		RegisterGlobalGameSystem();
		mGameWorld = GameObjectFactory::GetInstance().CreateObject<GameWorld>();
		mGameWorld->SetGlobalGameSystemList(mGameSystemList); 
		auto camNode = mGameWorld->AddNode("EditorCamera");
		auto editorCam = camNode->AddComponent<EditorCamera>();
		Camera::SetMainCamera(editorCam);
		//LoadGameWrold();
	}
	void GameLogicSystemLayer::Destroy()
	{

	}
	void GameLogicSystemLayer::OnEvent(IEvent& e)
	{
		EventDispatcher eventDispatcher(e);
		eventDispatcher.Dispatch<RequestGetGameWorldEvent>(EVENT_BIND_FN(&GameLogicSystemLayer::ResponseGetGameWorld));
		eventDispatcher.Dispatch<NotifyRenderingReadyCompeleteEvent>(EVENT_BIND_FN(&GameLogicSystemLayer::ResponseNotfyRenderingReadyCompelete));
		eventDispatcher.Dispatch<NotifyEditorSceneOnClickEvent>(EVENT_BIND_FN(&GameLogicSystemLayer::ResponseEditorSceneOnClickEvent));
	}
	String GameLogicSystemLayer::GetLayerName()
	{
		return "GameLogicSystemLayer";
	}
	bool GameLogicSystemLayer::ResponseGetGameWorld(RequestGetGameWorldEvent& e)
	{
		e.GameWorld = mGameWorld;
		return true;
	}
	bool GameLogicSystemLayer::ResponseNotfyRenderingReadyCompelete(NotifyRenderingReadyCompeleteEvent& e)
	{
		if (mGameWorld != nullptr)
		{
			auto renderItem = mGameWorld->PushRenderItem();
			if (renderItem != nullptr)
			{
				RequestPushRenderItemEvent e;
				e.RenderItem = renderItem;
				Application::GetInstance().SendEvent(e);
			}
			auto lightItem = mGameWorld->PushLightItem();
			if (lightItem != nullptr)
			{
				RequestPushLightItemEvent e;
				e.LightItem = lightItem;
				Application::GetInstance().SendEvent(e);
			}
		}
		return true;
	}
	bool GameLogicSystemLayer::ResponseEditorSceneOnClickEvent(NotifyEditorSceneOnClickEvent& e)
	{
		NotifySelectedGameNodeInEditorEvent pickingEvent;
		if (mGameWorld != nullptr)
		{
			List<IJGObject*> exceptObjectList;
			auto inspectorView = UIManager::GetInstance().GetUIView<InspectorView>();
			if (inspectorView && inspectorView->GetViewModel())
			{
				exceptObjectList.push_back(inspectorView->GetViewModel()->GetTargetObject());
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


	}
	//void GameLogicSystemLayer::SaveGameWorld()
	//{
	//	if (mGameWorld == nullptr)
	//	{
	//		return;
	//	}
	//	static SharedPtr<ScheduleHandle> handle = nullptr;
	//	Scheduler::GetInstance().ScheduleOnce(0.0f, 0, [&]() -> EScheduleResult
	//	{
	//		auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
	//		if (progressBar->IsOpen() == false)
	//		{
	//			UIManager::GetInstance().OpenPopupUIView<ProgressBarModalView>(ProgressBarInitData("Save World"));
	//			progressBar->Display("Save.. ", 0.5f);

	//			handle = Scheduler::GetInstance().ScheduleAsync([&](void* data)
	//			{
	//				auto savePath = CombinePath(Application::GetAssetPath(),"testGameWorld") + JG_ASSET_FORMAT;
	//				if (mGameWorld != nullptr)
	//				{
	//					auto json = CreateSharedPtr<Json>();
	//					json->AddMember(JG_ASSET_FORMAT_KEY, (u64)EAssetFormat::GameWorld);
	//					auto assetJson = json->CreateJsonData();
	//					mGameWorld->MakeJson(assetJson);
	//					json->AddMember(JG_ASSET_KEY, assetJson);
	//					Json::Write(savePath, json);
	//				}
	//				else
	//				{
	//					JG_CORE_ERROR("Failed Save GameWorld : GameWorld is null");
	//				}
	//			});
	//		}
	//		Scheduler::GetInstance().Schedule(0.1f, 0.2f, -1, 0,  [&]()->EScheduleResult
	//		{
	//			// 완성했으면 
	//			if (handle->GetState() == EScheduleState::Compelete)
	//			{
	//				auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
	//				progressBar->Display("Compete", 1.0f);


	//				handle->Reset();
	//				handle = nullptr;

	//				// 0.5f 뒤에 완료
	//				Scheduler::GetInstance().ScheduleOnce(0.1f, 0, [&]()->EScheduleResult
	//				{
	//					auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
	//					progressBar->Close();
	//					JG_CORE_INFO("Comepete Save GameWorld");
	//					return EScheduleResult::Break;
	//				});
	//				return EScheduleResult::Break;
	//			}

	//

	//			return EScheduleResult::Continue;
	//		});

	//		return EScheduleResult::Continue;
	//	});
	//}
	//void GameLogicSystemLayer::LoadGameWrold()
	//{
	//	static GameWorld* newGameWorld = nullptr;
	//	static GameWorld* oldGameWorld = nullptr;
	//	static bool is_LoadSucess = false;
	//	// 갈아 치우고
	//	// 
	//	static SharedPtr<ScheduleHandle> handle = nullptr;
	//	Scheduler::GetInstance().ScheduleOnce(0.0f, 0, [&]() -> EScheduleResult
	//	{
	//		auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
	//		
	//		if (progressBar->IsOpen() == false)
	//		{
	//			UIManager::GetInstance().OpenPopupUIView<ProgressBarModalView>(ProgressBarInitData("Load World"));
	//			progressBar->Display("Load.. ", 0.5f);

	//			handle = Scheduler::GetInstance().ScheduleAsync([&](void* data)
	//			{
	//				auto loadPath = CombinePath(Application::GetAssetPath(), "testGameWorld") + JG_ASSET_FORMAT;
	//				auto json = CreateSharedPtr<Json>();
	//				if (Json::Read(loadPath, json) == true)
	//				{
	//					EAssetFormat assetFormat = EAssetFormat::None;
	//					auto assetFormatVal = json->GetMember(JG_ASSET_FORMAT_KEY);
	//					if (assetFormatVal)
	//					{
	//						assetFormat = (EAssetFormat)assetFormatVal->GetUint64();
	//					}

	//					if (EAssetFormat::GameWorld == assetFormat)
	//					{
	//						auto assetVal = json->GetMember(JG_ASSET_KEY);
	//						if (assetVal)
	//						{
	//							newGameWorld = GameObjectFactory::GetInstance().CreateObject<GameWorld>();
	//							newGameWorld->SetGlobalGameSystemList(mGameSystemList);
	//							newGameWorld->LoadJson(assetVal);
	//							is_LoadSucess = true;
	//						}
	//					}
	//				}
	//			});
	//		}
	//		Scheduler::GetInstance().Schedule(0.1f, 0.2f, -1, 0, [&]()->EScheduleResult
	//		{
	//			
	//			// 완성했으면 
	//			if (handle->GetState() == EScheduleState::Compelete)
	//			{
	//				if (is_LoadSucess)
	//				{
	//					auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
	//					progressBar->Display("Compete", 1.0f);

	//					oldGameWorld = mGameWorld;
	//					mGameWorld = newGameWorld;
	//					NotifyChangeGameWorldEvent e;
	//					e.GameWorld = mGameWorld;
	//					Application::GetInstance().SendEvent(e);

	//					handle->Reset();
	//					handle = nullptr;
	//					newGameWorld = nullptr;

	//					// 0.5f 뒤에 완료
	//					Scheduler::GetInstance().ScheduleOnce(0.1f, 0, [&]()->EScheduleResult
	//					{
	//						auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
	//						progressBar->Close();
	//						JG_CORE_INFO("Comepete Save GameWorld");

	//						return EScheduleResult::Break;
	//					});
	//					Scheduler::GetInstance().ScheduleOnceByFrame(1, 0, [&]() ->EScheduleResult
	//					{
	//						if (oldGameWorld)
	//						{
	//							oldGameWorld->Destroy(oldGameWorld);
	//						}
	//						return EScheduleResult::Break;
	//					});
	//				}
	//				else
	//				{
	//					auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
	//					progressBar->Display("Failed", 1.0f);
	//					Scheduler::GetInstance().ScheduleOnce(0.1f, 0, [&]()->EScheduleResult
	//					{
	//						auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
	//						progressBar->Close();
	//						JG_CORE_INFO("Comepete Load GameWorld");

	//						return EScheduleResult::Break;
	//					});
	//				}



	//				return EScheduleResult::Break;
	//			}



	//			return EScheduleResult::Continue;
	//		});
	//		return EScheduleResult::Continue;
	//	});



	//}
}