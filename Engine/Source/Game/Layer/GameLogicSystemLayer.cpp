#include "pch.h"
#include "GameLogicSystemLayer.h"
#include "Application.h"
#include "GameObject.h"
#include "GameNode.h"
#include "GameWorld.h"
#include "GameComponent.h"
#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Components/SpriteRenderer.h"
#include "Components/StaticMeshRenderer.h"
#include "Components/PointLight.h"
#include "Components/Collision.h"
#include "Components/SkyDome.h"
#include "Components/DevComponent.h"
#include "Manager/GameLayerManager.h"
#include "Class/Asset/Asset.h"
#include "Linker/GamePluginLinker.h"

#define GAME_DLL_NAME "SandBox_Game.dll"


namespace JG
{
	void GameLogicSystemLayer::OnAttach()
	{
		GameLayerManager::Create();
		GameObjectFactory::Create();
	}
	void GameLogicSystemLayer::OnDetach()
	{

		GameObjectFactory::Destroy();
		GameLayerManager::Destroy();
	}
	void GameLogicSystemLayer::Begin()
	{
		//UIManager::GetInstance().RegisterMainMenuItem("File/Save GameWorld %_S", 0, [&](){

		//	if (mGameWorld == nullptr) return;
		//	if (mIsGamePlaying == true) return;

		//	RequestSaveGameWorldEvent e;
		//	Application::GetInstance().SendEvent(e);

		//}, nullptr);

		mGamePlugin = CreateUniquePtr<Plugin>(GAME_DLL_NAME);
		mGamePlugin->Link(CreateSharedPtr<GamePluginLinker>());
		if (mGamePlugin->IsVaild())
		{
			JG_INFO("Successed Connect Game Plugin");
			auto func = mGamePlugin->LoadFunction<void>("TestFunction");
			if (func.IsVaild())
			{
				func();
			}
		}

		RegisterGameObjectType();
		RegisterGlobalGameSystem();

		//Test
		Scheduler::GetInstance().ScheduleOnceByFrame(100, 0, [&]() -> EScheduleResult {Test(); return EScheduleResult::Continue; });
		
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
		if (mGameWorld == nullptr || mGameWorldAssetPath.empty())
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
		GameObjectFactory::GetInstance().RegisterComponentType<PointLight>();
		GameObjectFactory::GetInstance().RegisterComponentType<SkyDome>();
		GameObjectFactory::GetInstance().RegisterComponentType<DevComponent>();
	}


	// Cluster Shading Test Function
	// 값 검증 후 Compute Shader로 옮기자.
	void GameLogicSystemLayer::Test()
	{
		auto mainCam = Camera::GetMainCamera();
		if (mainCam == nullptr)
		{
			return;
		}
		auto resolution = mainCam->GetResolution();
		auto viewMatrix = mainCam->GetViewMatrix();
		auto projMatrix = mainCam->GetProjMatrix();
		auto invProjMatrix = mainCam->GetInvProjMatrix();
		auto farZ = mainCam->GetFarZ();
		auto nearZ = mainCam->GetNearZ();
		auto eyePosition = mainCam->GetOwner()->GetTransform()->GetWorldLocation();

		// cluster shading test
		struct Cluster
		{
			JVector3 Min; // x,y, depth
			JVector3 Max; // x,y, depth
		};
		// depth -> [0 ... 1]
		// z Slice
		// N * N 의 Tile에서 N개의 깊이만큼 가진다.

		// 21 * 9 * 32
		i32 numXSlice = 21;
		i32 numYSlice = 9;
		i32 numZSlice = 32;

		JVector2 tileSize = JVector2(resolution.x / numXSlice, resolution.y / numYSlice);

		List<Cluster> clusters;


		std::function<JVector3(const JVector4&)> ScreenToView = [&](const JVector4& screen)->JVector3
		{

			JVector2 texCoord = JVector2(screen.x / resolution.x, screen.y / resolution.y);
			texCoord = JVector2(texCoord.x, 1.0f - texCoord.y) * 2.0f - JVector2(1.0f, 1.0f);
			JVector4 clip = JVector4(texCoord.x, texCoord.y, screen.z, screen.w);
			clip = invProjMatrix.Transform(clip);
			clip = clip / clip.w;
			return clip;
		};
		std::function<JVector3(const JVector3&, const JVector3&, f32)> LineIntersectionToZPlane =
			[&](const JVector3& A, const JVector3& B, f32 zDistance) -> JVector3
		{
			JVector3 normal = JVector3(0, 0, 1);
			JVector3 ab = B - A;
			f32 t = (zDistance - JVector3::Dot(normal, A)) / JVector3::Dot(normal, ab);

			JVector3 result = A + t * ab;
			return result;
		};


		for (i32 i = 0; i < numZSlice; ++i)
		{
			Cluster cluster;
			
			f32 cameraScale = 0.0f; // 
			f32 clusterNear = nearZ * std::pow((farZ / nearZ), (f32)i / (f32)numZSlice);; //
			f32 clusterFarZ = nearZ * std::pow((farZ / nearZ), (f32)(i + 1) / (f32)numZSlice);


			for (i32 x = 0; x < numXSlice; ++x)
			{
				for (i32 y = 0; y < numYSlice; ++y)
				{
					JVector4 minPoint_Ss = JVector4((f32)x * tileSize.x, (f32)y * tileSize.y, -1, 1);
					JVector4 maxPoint_Ss = JVector4(tileSize.x, tileSize.y, -1, 1);
										
					JVector3 maxPoint_vS = ScreenToView(maxPoint_Ss);
					JVector3 minPoint_vS = ScreenToView(minPoint_Ss);


					JVector3 minPointNear = LineIntersectionToZPlane(eyePosition, minPoint_vS, clusterNear);
					JVector3 minPointFar  = LineIntersectionToZPlane(eyePosition, minPoint_vS, clusterFarZ);
					JVector3 maxPointNear = LineIntersectionToZPlane(eyePosition, maxPoint_vS, clusterNear);
					JVector3 maxPointFar  = LineIntersectionToZPlane(eyePosition, maxPoint_vS, clusterFarZ);

					
					JVector3 minPointAABB = JVector3::Min(JVector3::Min(minPointNear, minPointFar), JVector3::Min(maxPointNear, maxPointFar));
					JVector3 maxPointAABB = JVector3::Max(JVector3::Max(minPointNear, minPointFar), JVector3::Max(maxPointNear, maxPointFar));

					cluster.Min = minPointAABB;
					cluster.Max = maxPointAABB;
					clusters.push_back(cluster);
				}
			}
			// xSlice
			// NDC
			// [-1 .. 1]

		}





		









	}

}