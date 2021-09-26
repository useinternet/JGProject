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
#include "Class/UI/ModalUI/MessageBoxModalView.h"
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

		UIManager::GetInstance().RegisterMainMenuItem("File/Save GameWorld %_S", 0, [&](){

			if (mGameWorld == nullptr) return;


			RequestSaveGameWorldEvent e;
			Application::GetInstance().SendEvent(e);



		}, nullptr);


		RegisterGameObjectType();
		RegisterGlobalGameSystem();

	
		mGameWorld = GameObjectFactory::GetInstance().CreateObject<GameWorld>();
		mGameWorld->SetGlobalGameSystemList(mGameSystemList); 
		auto camNode = mGameWorld->AddNode("EditorCamera");
		auto editorCam = camNode->AddComponent<EditorCamera>();
		Camera::SetMainCamera(editorCam);

		NotifyChangeGameWorldEvent e;
		e.GameWorld = mGameWorld;
		Application::GetInstance().SendEvent(e);



	

		//LoadGameWrold();
	}
	void GameLogicSystemLayer::Destroy()
	{

	}
	void GameLogicSystemLayer::OnEvent(IEvent& e)
	{
		EventDispatcher eventDispatcher(e);
		eventDispatcher.Dispatch<RequestSaveGameWorldEvent>(EVENT_BIND_FN(&GameLogicSystemLayer::ResponseSaveGameWorld));
		eventDispatcher.Dispatch<RequestLoadGameWorldEvent>(EVENT_BIND_FN(&GameLogicSystemLayer::ResponseLoadGameWorld));
		eventDispatcher.Dispatch<NotifyRenderingReadyCompeleteEvent>(EVENT_BIND_FN(&GameLogicSystemLayer::ResponseNotfyRenderingReadyCompelete));
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
				if (mGameWorldJsonPath.empty())
				{
					data->Path = CombinePath(Application::GetAssetPath(), "NewGameWorld.jgasset");
				}
				else
				{
					data->Path = mGameWorldJsonPath;
				}


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
				progressBar->Close();
				if (data->IsSuccessed)
				{

				}
				else
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
	bool GameLogicSystemLayer::ResponseLoadGameWorld(RequestLoadGameWorldEvent& e)
	{
		if (fs::exists(e.AssetPath) == false)
		{
			JG_CORE_ERROR("is not exist GameWorld Asset, Fail Import GameWorld");
			return true;
		}
		if (mLoadGameWorldTaskCtrl && mLoadGameWorldTaskCtrl->IsRun())
		{
			return true;
		}
		enum
		{
			Task_Seq_ReadJson,
			Task_Seq_LoadJson,
		};


		auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
		UIManager::GetInstance().OpenPopupUIView<ProgressBarModalView>(ProgressBarInitData("Load GameWorld"));
		mLoadGameWorldTaskCtrl = CreateUniquePtr<TaskController>();
		mLoadGameWorldTaskCtrl->SetPriority(SchedulePriority::EndSystem);
		mLoadGameWorldTaskCtrl->SetStartID(Task_Seq_ReadJson);
		mLoadGameWorldTaskCtrl->SetUserData(CreateSharedPtr<LoadGameWorldData>(e.AssetPath));

		Scheduler::GetInstance().ScheduleOnce(0.15f, 0, [&]() -> EScheduleResult
		{
			mLoadGameWorldTaskCtrl->AddTask(Task_Seq_ReadJson, nullptr,
				[&](SharedPtr<IJGObject> userData)
			{
				auto data = userData->As<LoadGameWorldData>();
				auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
				progressBar->Display("Read Json...", 0.25f);
				data->Json = CreateSharedPtr<Json>();
				EAssetFormat format;

				data->IsSuccessed = AssetDataBase::GetInstance().ReadAsset(data->Path, &format, &(data->Json)) && format == EAssetFormat::GameWorld;
			}, [&](SharedPtr<IJGObject> userData) -> i32
			{
				auto data = userData->As<LoadGameWorldData>();
				if (data->IsSuccessed == false)
				{
					JG_CORE_ERROR("Fail Read Asset, Fail Load GameWorld");
					return -1;
				}
				else
				{
					mGameWorldJsonPath = data->Path;
				}
				return Task_Seq_LoadJson;
			});


			mLoadGameWorldTaskCtrl->AddTask(Task_Seq_LoadJson, nullptr,
				[&](SharedPtr<IJGObject> userData)
			{

				auto data = userData->As<LoadGameWorldData>();
				auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
				progressBar->Display("Load Json...", 0.75f);
				auto jsonData = data->Json->GetMember(JG_ASSET_KEY);
				if (jsonData)
				{
					data->GameWorld = GameObjectFactory::GetInstance().CreateObject<GameWorld>();
					data->GameWorld->LoadJson(jsonData);
					data->IsSuccessed = true;
				}
				else
				{
					data->IsSuccessed = false;
				}
			}, [&](SharedPtr<IJGObject> userData) -> i32
			{
				auto data = userData->As<LoadGameWorldData>();
				auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
				progressBar->Display("Load Compelete", 1.0f);
				progressBar->Close();
				if (data->IsSuccessed)
				{
					if (mGameWorld)
					{
						GameObject::DestoryObject(mGameWorld);
						mGameWorld = nullptr;
					}
					mGameWorld = data->GameWorld;
					NotifyChangeGameWorldEvent e;
					e.GameWorld = mGameWorld;
					Application::GetInstance().SendEvent(e);

				}
				else
				{
					UIManager::GetInstance().OpenPopupUIView< MessageBoxModalView>(MessageBoxInitData("Error", "Fail Load GameWorld"));
				}

				return -1;
			});
			mLoadGameWorldTaskCtrl->Run();

			return EScheduleResult::Break;
		});

	
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
	//			// �ϼ������� 
	//			if (handle->GetState() == EScheduleState::Compelete)
	//			{
	//				auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
	//				progressBar->Display("Compete", 1.0f);


	//				handle->Reset();
	//				handle = nullptr;

	//				// 0.5f �ڿ� �Ϸ�
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
	//	// ���� ġ���
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
	//			// �ϼ������� 
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

	//					// 0.5f �ڿ� �Ϸ�
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