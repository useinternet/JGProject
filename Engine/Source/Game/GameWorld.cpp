#include "pch.h"
#include "GameWorld.h"
#include "Class/Asset/Asset.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
namespace JG
{
	GameWorld::GameWorld() : GameNode(this)
	{
		mAssetManager = AssetDataBase::GetInstance().RequestAssetManager();
		mFixedUpdateSH = ScheduleByFrame(0, 0, -1, 0, [&]() -> EScheduleResult
		{
			if (IsAlive() == false)
			{
				return EScheduleResult::Break;
			}
			FixedUpdate();
			return EScheduleResult::Continue;
		});
		mFixedLateUpdateSH = ScheduleByFrame(0, 0, -1, 0, [&]() -> EScheduleResult
		{
			if (IsAlive() == false)
			{
				return EScheduleResult::Break;
			}
			FixedLateUpdate();
			return EScheduleResult::Continue;
		});
		mUpdateSH = ScheduleByFrame(0, 0, -1, 0, [&]() -> EScheduleResult
		{
			if (IsAlive() == false)
			{
				return EScheduleResult::Break;
			}
			if (mGameWorldFlags & EGameWorldFlags::Update_Lock)
			{
				return EScheduleResult::Continue;
			}
			Update();
			return EScheduleResult::Continue;
		});
		mLateUpdateSH = ScheduleByFrame(0, 0, -1, 0, [&]() -> EScheduleResult
		{
			if (IsAlive() == false)
			{
				return EScheduleResult::Break;
			}
			if (mGameWorldFlags & EGameWorldFlags::LateUpdate_Lock)
			{
				return EScheduleResult::Continue;
			}
			LateUpdate();
			return EScheduleResult::Continue;
		});
		mPxSceneHandle = PhysicsManager::GetInstance().CreatePxScene();
	}
	void GameWorld::Awake()
	{
		GameNode::Awake();
	}
	void GameWorld::Update()
	{
		GameNode::Update();
	}
	void GameWorld::LateUpdate()
	{
		GameNode::LateUpdate();
	}
	void GameWorld::Destory()
	{
		GameNode::Destory();


		PhysicsManager::GetInstance().PushRemoveCommand(mPxSceneHandle);
		AssetDataBase::GetInstance().ReturnAssetManager(mAssetManager);
		mAssetManager = nullptr;
		mFixedUpdateSH->Reset();
		mFixedLateUpdateSH->Reset();
		mUpdateSH->Reset();
		mLateUpdateSH->Reset();
	
		mFixedUpdateSH = nullptr;
		mUpdateSH = nullptr;
		mLateUpdateSH = nullptr;
	}

	void GameWorld::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		GameNode::MakeJson(jsonData);
		jsonData->AddMember("Gravity", GetGravity());
	}
	void GameWorld::LoadJson(SharedPtr<JsonData> jsonData)
	{
		GameNode::LoadJson(jsonData);
		auto val = jsonData->GetMember("Gravity");
		if (val)
		{
			SetGravity(val->GetVector3());
		}
	}
	AssetManager* GameWorld::GetAssetManager() const
	{
		return mAssetManager.get();
	}
	void GameWorld::PushRenderSceneObject(SharedPtr<Graphics::SceneObject> sceneObject)
	{
		for (auto& scene_pair : mGraphicsSceneDic)
		{
			if (scene_pair.first->IsActive() == false)
			{
				return;
			}
			if (scene_pair.second->IsEnableRendering() == true)
			{
				scene_pair.second->PushSceneObject(sceneObject);
			}
	
		}
	}
	void GameWorld::PushRenderLightObject(SharedPtr<Graphics::Light> lightObject)
	{
		for (auto& scene_pair : mGraphicsSceneDic)
		{
			if (scene_pair.first->IsActive() == false)
			{
				return;
			}
			if (scene_pair.second->IsEnableRendering() == true)
			{
				scene_pair.second->PushLight(lightObject);
			}

		}
	}

	void GameWorld::RegisterGraphicsScene(GameComponent* com, Graphics::Scene* scene)
	{
		mGraphicsSceneDic.emplace(com, scene);
	}
	void GameWorld::UnRegisterGraphicsScene(GameComponent* com)
	{
		mGraphicsSceneDic.erase(com);
	}
	void GameWorld::AddGameSystem(const Type& type)
	{
		bool isGameSystem = GameObjectFactory::GetInstance().IsGameSystem(type);
		if (isGameSystem == false)
		{
			JG_LOG_ERROR("This Type({0}) is not GameSystem Category.", type.GetName());
			return;
		}
		auto obj = GameObjectFactory::GetInstance().CreateObjectByType(type);
		if (obj == nullptr)
		{
			JG_LOG_ERROR("This Type({0}) is not registered.", type.GetName());
			return;
		}
		auto sys = static_cast<GameSystem*>(obj);
		sys->mGameWorld = this;
		mWorldGameSystemList.push_back(sys);
	}
	void GameWorld::Destroy(GameSystem* sys)
	{
		GameObject::DestoryObject(sys);
	}
	void GameWorld::Destroy(GameWorld* world)
	{
		GameObject::DestoryObject(world);
	}
	void GameWorld::AddFlags(EGameWorldFlags flags)
	{
		mGameWorldFlags = mGameWorldFlags | flags;
	}
	void GameWorld::RemoveFlags(EGameWorldFlags flags)
	{
		mGameWorldFlags = (EGameWorldFlags)((i32)mGameWorldFlags & (~(i32)flags));
	}

	void GameWorld::SetGravity(const JVector3& gravity)
	{
		bool isDirty = mGravity != gravity;
		mGravity = gravity;
		if (isDirty)
		{
			PhysicsManager::GetInstance().PushWriteCommand<physx::PxScene>(
				mPxSceneHandle, [&](physx::PxScene* scene)
			{
				auto _garvity = GetGravity();
				scene->setGravity(physx::PxVec3(_garvity.x, _garvity.y, _garvity.z));
			});


		}
	}
	const JVector3& GameWorld::GetGravity() const
	{
		return mGravity;
	}

	f32 GameWorld::GetTickCycle() const
	{
		return mTickCycle;
	}

	PhysicsHandle GameWorld::GetPxSceneHandle() const
	{
		return mPxSceneHandle;
	}

	GameNode* GameWorld::Picking(const JVector2& screenPos, List<IJGObject*> exceptObjectList)
	{
		auto mainCam = Camera::GetMainCamera();
		if (mainCam == nullptr)
		{
			return nullptr;
		}
		GameNode* pickingObject = nullptr;
		List<GameNode*> pickingObjectList;
		if (mainCam->IsOrthographic() == false)
		{
			auto ray = mainCam->ScreenToWorldRay(JVector3(screenPos, 1.0f));
			Picking3DRecursive(pickingObjectList, ray);

			// 현재 카메라 에서의 거리순으로 정렬
			auto camPos = mainCam->GetOwner()->GetTransform()->GetWorldLocation();
			
			std::sort(pickingObjectList.begin(), pickingObjectList.end(), [&](GameNode* n1, GameNode* n2) -> bool
			{
				auto n1Distance = JVector3::LengthSqrd(n1->GetTransform()->GetWorldLocation() - camPos);
				auto n2Distance = JVector3::LengthSqrd(n2->GetTransform()->GetWorldLocation() - camPos);
				
				return n1Distance < n2Distance;
			});
		}
		else
		{
			auto p = mainCam->ScreenToWorldPoint(JVector3(screenPos, 1.0f));
			Picking2DRecursive(pickingObjectList, JVector2(p.x, p.y));

	
		}
		u64 cnt = 0;
		for (u64 i = 0; i < pickingObjectList.size(); ++i)
		{
			pickingObject = pickingObjectList[i];
			bool isExcept = false;
			for (auto& obj : exceptObjectList)
			{
				if (pickingObject == obj)
				{
					isExcept = true;
					break;
				}
			}

			if (isExcept == false)
			{
				break;
			}

		}




		return pickingObject;
	}
	void GameWorld::SetGlobalGameSystemList(const List<GlobalGameSystem*>& systemList)
	{
		mGlobalGameSystemList = systemList;
	}
	List<GlobalGameSystem*> GameWorld::GetGlobalGameSystemList() const
	{
		return mGlobalGameSystemList;
	}
}