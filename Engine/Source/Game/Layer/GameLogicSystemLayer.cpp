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


#include "Graphics/DebugGeometryDrawer.h"
#include "Graphics/Resource.h"
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
		static Color colors[4];
		colors[0] = Color::Red();
		colors[1] = Color::Green();
		colors[2] = Color::Blue();
		colors[3] = Color::White();
		Scheduler::GetInstance().ScheduleByFrame(10, 0, -1, 0, [&]() -> EScheduleResult {

			Test();
			i32 index = 0;
			for (auto& cluster : mClusters)
			{
				bool result = true; // index >= (numXSlice * numYSlice * 20) && index < (numXSlice* numYSlice * 21);
				if (result == true)
				{
					JVector3 location = cluster.Center();
					JVector3 size = JVector3::Abs(cluster.max - cluster.min);
					JQuaternion quat = JQuaternion::Identity();


					i32 colorIndex = index / (numXSlice * numYSlice);
					i32 xcolorIndex = (index / numYSlice) % numXSlice;
					i32 ycolorIndex = (index % numYSlice);
					colorIndex = colorIndex % 4;
					auto color = colors[colorIndex];
					if (color.R > 0)
					{
						color.R -= (0.25f * (f32)xcolorIndex);
					}
					else
					{
						color.R += (0.25f * (f32)xcolorIndex);
					}
					if (color.G > 0)
					{
						color.G -= (0.25f * (f32)ycolorIndex);
					}
					else
					{
						color.G += (0.25f * (f32)ycolorIndex);
					}

					JGGraphics::GetInstance().GetDebugGeometryDrawer()->DrawDebugBox(location, quat, size, color);

				}




				++index;
			}


			return EScheduleResult::Continue; });



		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::Graphics_EndFrame, [&]()->EScheduleResult
		{
			if (mComputer == nullptr)
			{
				auto mainCam = Camera::GetMainCamera();
				if (mainCam == nullptr)
				{
					return EScheduleResult::Continue;
				}
				constantData.__Resolution__ = mainCam->GetResolution();
				constantData.__ProjMatrix__ = JMatrix::PerspectiveFovLH(Math::ConvertToRadians(45), constantData.__Resolution__.x / constantData.__Resolution__.y, 0.1f, 100000);
				constantData.__InvProjMatrix__ = JMatrix::Inverse(constantData.__ProjMatrix__);
				constantData.__FarZ__ = 100000;
				constantData.__NearZ__ = 0.1f;
				constantData.__EyePosition__ = JVector3(0, 0, 0);// mainCam->GetOwner()->GetTransform()->GetWorldLocation();;
				auto shaderPath = Application::GetEnginePath();
				shaderPath = CombinePath(shaderPath, "Shader/Compute/ComputeCluster.shaderscript");
				String sourceCode;
				if (File::GetReadAllText(shaderPath, &sourceCode) == true)
				{
					auto shader = IShader::Create("ComputeTestShader", sourceCode, EShaderFlags::Allow_ComputeShader);
					mComputer = IComputer::Create("Test", shader);
					mComputer->SetFloat4x4("__ProjMatrix__", constantData.__ProjMatrix__);
					mComputer->SetFloat4x4("__InvProjMatrix__", constantData.__InvProjMatrix__);
					mComputer->SetFloat3("__EyePosition__", constantData.__EyePosition__);

					mComputer->SetFloat("__FarZ__", constantData.__FarZ__);
					mComputer->SetFloat("__NearZ__", constantData.__NearZ__);
					mComputer->SetFloat2("__Resolution__", constantData.__Resolution__);
					mComputer->Dispatch(1, 1, 1);
				}
			}
			else
			{
				if (mComputer->GetState() == EComputerState::Compelete && mReadBackBuffer == nullptr)
				{
					auto rwBuffer = mComputer->GetRWBuffer("Test");
					mReadBackBuffer = IReadBackBuffer::Create("TestReadBack", rwBuffer);
				}
				if (mReadBackBuffer && mReadBackBuffer->GetState() == EReadBackBufferState::ReadCompelete)
				{
					u64 dataSize = mReadBackBuffer->GetDataSize();
					dataSize = dataSize / sizeof(JVector3);
					List<JVector3> _list;
					_list.resize(1024);
					mReadBackBuffer->GetData(_list.data(), sizeof(JVector3) * 1024);
				}
				

			}
			return EScheduleResult::Continue;
		});



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
		auto viewMatrix = JMatrix::LookAtLH(JVector3(0, 0, 0), JVector3(0, 0, 1), JVector3(0, 1, 0));
		auto projMatrix = JMatrix::PerspectiveFovLH(Math::ConvertToRadians(45), resolution.x / resolution.y, 0.1f, 100000);
		auto invProjMatrix = JMatrix::Inverse(projMatrix);
		auto nearZ = 0.1f;
		auto farZ = 100000;
		auto eyePosition = JVector3(0, 0, 0);// mainCam->GetOwner()->GetTransform()->GetWorldLocation();

		constantData.__Resolution__ = resolution;
		constantData.__ProjMatrix__ = projMatrix;
		constantData.__InvProjMatrix__ = invProjMatrix;
		constantData.__FarZ__ = farZ;
		constantData.__NearZ__ = nearZ;
		constantData.__EyePosition__ = eyePosition;
		// depth -> [0 ... 1]
		// z Slice
		// N * N 의 Tile에서 N개의 깊이만큼 가진다.


		//i32 numXSlice = 21;
		//i32 numYSlice = 9;
		//i32 numZSlice = 32;


		JVector2 tileSize = JVector2(resolution.x / numXSlice, resolution.y / numYSlice);

		List<JBBox> clusters;


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

		i32 index = 0;

		for (i32 i = 0; i < numZSlice; ++i)
		{
			JBBox cluster;
			
			f32 cameraScale = 0.0f; // 
			f32 clusterNear = nearZ * std::pow((farZ / nearZ), (f32)i / (f32)numZSlice);; //
			f32 clusterFarZ = nearZ * std::pow((farZ / nearZ), (f32)(i + 1) / (f32)numZSlice);

			for (i32 x = 0; x < numXSlice; ++x)
			{
				for (i32 y = 0; y < numYSlice; ++y)
				{
					bool result = index >= (numXSlice * numYSlice * 20) && index < (numXSlice* numYSlice * 21);
					if (result == true)
					{
						int n = 0;
					}

					JVector4 minPoint_Ss = JVector4((f32)x * tileSize.x, (f32)y * tileSize.y, -1, 1);
					JVector4 maxPoint_Ss = JVector4((f32)(x + 1) * tileSize.x, (f32)(y + 1) * tileSize.y, -1, 1);
										
					JVector3 maxPoint_vS = ScreenToView(maxPoint_Ss);
					JVector3 minPoint_vS = ScreenToView(minPoint_Ss);


					JVector3 minPointNear = LineIntersectionToZPlane(eyePosition, minPoint_vS, clusterNear);
					JVector3 minPointFar  = LineIntersectionToZPlane(eyePosition, minPoint_vS, clusterFarZ);
					JVector3 maxPointNear = LineIntersectionToZPlane(eyePosition, maxPoint_vS, clusterNear);
					JVector3 maxPointFar  = LineIntersectionToZPlane(eyePosition, maxPoint_vS, clusterFarZ);

					JVector3 minPointAABB = JVector3::Min(JVector3::Min(minPointNear, minPointFar), JVector3::Min(maxPointNear, maxPointFar));
					JVector3 maxPointAABB = JVector3::Max(JVector3::Max(minPointNear, minPointFar), JVector3::Max(maxPointNear, maxPointFar));

					cluster.min = JVector4(minPointAABB, 0.0f);
					cluster.max = JVector4(maxPointAABB, 0.0f);

					clusters.push_back(cluster);


					++index;
				}
			}
		}





		mClusters = clusters;
	}

}