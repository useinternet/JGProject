#include "pch.h"
#include "Collision.h"
#include "Class/Game/GameNode.h"
#include "Transform.h"
#include "Class/Game/GameWorld.h"
namespace JG
{

	void Collision::Awake()
	{
	}
	void Collision::Start()
	{
		ScheduleByFrame(0, 0, -1, 0, [&]()-> EScheduleResult
		{
			if (mIsShapeDirty)
			{
				mIsShapeDirty = false;
				RemoveShape();
				CreateShape();
			}

			UpdateRigid();

			if (mIsDebugDraw == true)
			{
				PhysicsManager::GetInstance().PxRigidStaticReadWrite(mRigidHandle, [&](physx::PxRigidStatic* rigidStatic)
				{
					DrawDebugShape(rigidStatic);
				});
			}
			return EScheduleResult::Continue;
		});
		CreateShape();
		CreateRigid();
	}

	void Collision::Destory()
	{
		RemoveRigid();
		RemoveShape();
	}

	void Collision::Update()
	{
	}

	void Collision::LateUpdate()
	{
	}

	void Collision::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		GameComponent::MakeJson(jsonData);
		jsonData->AddMember("Center", GetCenter());
		jsonData->AddMember("IsDebugDraw", mIsDebugDraw);
	}

	void Collision::LoadJson(SharedPtr<JsonData> jsonData)
	{
		GameComponent::LoadJson(jsonData);

		auto val = jsonData->GetMember("Center");
		if (val)
		{
			SetCenter(val->GetVector3());
		}

		val = jsonData->GetMember("IsDebugDraw");
		if (val && val->IsBool())
		{
			mIsDebugDraw = val->GetBool();
		}
	}

	void Collision::OnInspectorGUI()
	{
		GameComponent::OnInspectorGUI();
		auto center = GetCenter();

		auto label_space = ImGui::CalcTextSize("DebugDraw").x;
		ImGui::Bool_OnGUI("DebugDraw", mIsDebugDraw, label_space);
		ImGui::Vector3_OnGUI("Center", center, label_space);


		SetCenter(center);
	}

	void Collision::CreateRigid()
	{
		if (mRigidHandle != 0)
		{
			RemoveRigid();
		}
		mIsRigidDirty = true;

		if (mIsDynamicRigid == false)
		{
			mRigidHandle = PhysicsManager::GetInstance().CreatePxRigidStatic();
			auto sceneHandle = GetGameWorld()->GetPxSceneHandle();
			PhysicsManager::GetInstance().PxSceneReadWrite(sceneHandle, [&](physx::PxScene* scene)
			{
				PhysicsManager::GetInstance().PxRigidStaticReadWrite(mRigidHandle, [&](physx::PxRigidStatic* rigidStatic)
				{
					scene->addActor(*rigidStatic);
				});
			});
		}
		else
		{
			mRigidHandle = PhysicsManager::GetInstance().CreatePxRigidDynamic();
			auto sceneHandle = GetGameWorld()->GetPxSceneHandle();
			PhysicsManager::GetInstance().PxSceneReadWrite(sceneHandle, [&](physx::PxScene* scene)
			{
				PhysicsManager::GetInstance().PxRigidDynamicSceneReadWrite(mRigidHandle, [&](physx::PxRigidDynamic* rigidDynamic)
				{
					scene->addActor(*rigidDynamic);
				});
			});
		}


		UpdateRigid();
	}

	void Collision::UpdateRigid()
	{

		if (mIsRigidDirty)
		{
			mIsRigidDirty = false;
			auto location = GetOwner()->GetTransform()->GetWorldLocation() + GetCenter();
			//auto rotation = GetOwner()->GetTransform()->GetWorldQuat();


			physx::PxTransform transform;
			transform.p = physx::PxVec3(location.x, location.y, location.z);
			//transform.q = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);


			if (mIsDynamicRigid)
			{
				PhysicsManager::GetInstance().PxRigidDynamicSceneReadWrite(mRigidHandle, [&](physx::PxRigidDynamic* rigidDynamic)
				{
					rigidDynamic->setGlobalPose(transform);
				});
			}
			else
			{
				PhysicsManager::GetInstance().PxRigidStaticReadWrite(mRigidHandle, [&](physx::PxRigidStatic* rigidStatic)
				{
					rigidStatic->setGlobalPose(transform);
				});
			}
		}

		if (mIsDynamicRigid)
		{

			PhysicsManager::GetInstance().PxRigidDynamicSceneReadWrite(mRigidHandle, [&](physx::PxRigidDynamic* rigidDynamic)
			{
				auto pose = rigidDynamic->getGlobalPose();



			});




		}



	}

	void Collision::RemoveRigid()
	{
		auto sceneHandle = GetGameWorld()->GetPxSceneHandle();

		if (mIsDynamicRigid == true)
		{
			PhysicsManager::GetInstance().PxSceneReadWrite(sceneHandle, [&](physx::PxScene* scene)
			{
				PhysicsManager::GetInstance().PxRigidDynamicSceneReadWrite(mRigidHandle, [&](physx::PxRigidDynamic* rigidDynamic)
				{
					scene->removeActor(*rigidDynamic);
				});
			});

			PhysicsManager::GetInstance().RemovePxRigidDynamic(mRigidHandle);
		}
		else
		{
			PhysicsManager::GetInstance().PxSceneReadWrite(sceneHandle, [&](physx::PxScene* scene)
			{
				PhysicsManager::GetInstance().PxRigidStaticReadWrite(mRigidHandle, [&](physx::PxRigidStatic* rigidStatic)
				{
					scene->removeActor(*rigidStatic);
				});
			});

			PhysicsManager::GetInstance().RemovePxRigidStatic(mRigidHandle);
		}

		mRigidHandle = 0;
	}

	void Collision::CreateShape()
	{
		if (mShapeHandle != 0)
		{
			RemoveShape();
		}
		mShapeHandle = PhysicsManager::GetInstance().CreatePxShape([&]() -> PxShapeData
		{
			PxShapeData data;
			data.Geometry = CreatePxGeometry();
			return data;
		});

		PhysicsManager::GetInstance().PxRigidStaticReadWrite(mRigidHandle, [&](physx::PxRigidStatic* rigidStatic)
		{
			PhysicsManager::GetInstance().PxShapeSceneReadWrite(mShapeHandle, [&](physx::PxShape* shape, physx::PxMaterial* material)
			{
				rigidStatic->attachShape(*shape);
			});
		});
	}

	void Collision::RemoveShape()
	{
		PhysicsManager::GetInstance().PxRigidStaticReadWrite(mRigidHandle, [&](physx::PxRigidStatic* rigidStatic)
		{
			PhysicsManager::GetInstance().PxShapeSceneReadWrite(mShapeHandle, [&](physx::PxShape* shape, physx::PxMaterial* material)
			{
				rigidStatic->detachShape(*shape);
			});
		});

		PhysicsManager::GetInstance().RemovePxShape(mShapeHandle);
		mShapeHandle = 0;
	}

	void Collision::SendShapeDirty()
	{
		mIsShapeDirty = true;
	}


	void Collision::SetCenter(const JVector3& center)
	{
		mCenter = center;
	}

	const JVector3& Collision::GetCenter() const
	{
		return mCenter;
	}




	void SphereCollision::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		Collision::MakeJson(jsonData);

		jsonData->AddMember("Radius", GetRadius());
	}

	void SphereCollision::LoadJson(SharedPtr<JsonData> jsonData)
	{
		Collision::LoadJson(jsonData);
		auto val = jsonData->GetMember("Radius");

		if (val && val->IsFloat())
		{
			SetRadius(val->GetFloat());
		}
	}
	void SphereCollision::OnInspectorGUI()
	{
		Collision::OnInspectorGUI();
		auto radius = GetRadius();
		auto label_space = ImGui::CalcTextSize("DebugDraw").x;
		ImGui::Float_OnGUI("Radius", radius, label_space);
		SetRadius(radius);
	}

	SharedPtr<physx::PxGeometry> SphereCollision::CreatePxGeometry()
	{
		return CreateSharedPtr<physx::PxSphereGeometry>(GetRadius());
	}

	void SphereCollision::DrawDebugShape(physx::PxRigidStatic* rigidStatic)
	{
		auto l = rigidStatic->getGlobalPose().p;

		DrawDebugSphere(JVector3(l.x, l.y, l.z), GetRadius(), GetDebugColor());
	}

	
	void SphereCollision::SetRadius(f32 r)
	{
		if (mRadius != r)
		{
			SendShapeDirty();
		}
		mRadius = r;
	}

	f32 SphereCollision::GetRadius() const
	{
		return mRadius;
	}



	void BoxCollision::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		Collision::MakeJson(jsonData);
		jsonData->AddMember("Size", GetSize());
	}
	void BoxCollision::LoadJson(SharedPtr<JsonData> jsonData)
	{
		Collision::LoadJson(jsonData);
		auto val = jsonData->GetMember("Size");

		if (val)
		{
			SetSize(val->GetVector3());
		}
	}
	void BoxCollision::OnInspectorGUI()
	{
		Collision::OnInspectorGUI();
		auto size = GetSize();
		auto label_space = ImGui::CalcTextSize("DebugDraw").x;
		ImGui::Vector3_OnGUI("Size", size, label_space);
		SetSize(size);
	}
	SharedPtr<physx::PxGeometry> BoxCollision::CreatePxGeometry()
	{
		auto halfSize = GetSize() * 0.5f;
		return CreateSharedPtr<physx::PxBoxGeometry>(halfSize.x, halfSize.y, halfSize.z);
	}
	void BoxCollision::DrawDebugShape(physx::PxRigidStatic* rigidStatic)
	{

		auto l = rigidStatic->getGlobalPose().p;
		auto q = rigidStatic->getGlobalPose().q;
		DrawDebugBox(JVector3(l.x, l.y, l.z), JQuaternion(q.x, q.y, q.z, q.w), GetSize(), GetDebugColor());

	}
	void BoxCollision::SetSize(const JVector3& size)
	{
		mSize = size;
		SendShapeDirty();
	}
	const JVector3& BoxCollision::GetSize() const
	{
		return mSize;
	}
}