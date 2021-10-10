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
			UpdateRigid();

			if (mIsDebugDraw == true)
			{
				if (mIsDynamicRigid)
				{
					PhysicsManager::GetInstance().PxRigidDynamicReadWrite(mRigidHandle, [&](physx::PxRigidDynamic* rigidDynamic)
					{
						DrawDebugShape(rigidDynamic);
					});
				}
				else
				{
					PhysicsManager::GetInstance().PxRigidStaticReadWrite(mRigidHandle, [&](physx::PxRigidStatic* rigidStatic)
					{
						DrawDebugShape(rigidStatic);
					});
				}

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
		jsonData->AddMember("IsDebugDraw", IsDebugDraw());
		jsonData->AddMember("IsSimulate", IsSimulate());
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
			SetDebugDraw(val->GetBool());
		}
		val = jsonData->GetMember("IsSimulate");
		if (val && val->IsBool())
		{
			SetSimulate(val->GetBool());
		}
	}

	void Collision::OnInspectorGUI()
	{
		GameComponent::OnInspectorGUI();
		auto center = GetCenter();

		auto label_space = ImGui::CalcTextSize("DebugDraw").x;
		bool isSimulate = IsSimulate();
		bool isDebugDraw = IsDebugDraw();
		ImGui::Bool_OnGUI("Debug Draw", isDebugDraw, label_space);
		ImGui::Bool_OnGUI("IsSimulate", isSimulate, label_space);
		ImGui::Vector3_OnGUI("Center", center, label_space);


		SetSimulate(isSimulate);
		SetCenter(center);
		SetDebugDraw(isDebugDraw);
	}

	void Collision::CreateRigid()
	{
		if (mRigidHandle != 0)
		{
			RemoveRigid();
		}

		auto location = GetOwner()->GetTransform()->GetWorldLocation() + GetCenter();
		auto rotation = GetOwner()->GetTransform()->GetWorldQuaternion();
		if (mIsDynamicRigid == false)
		{
			mRigidHandle = PhysicsManager::GetInstance().CreatePxRigidStatic(location, rotation);
			auto sceneHandle = GetGameWorld()->GetPxSceneHandle();
			PhysicsManager::GetInstance().PxSceneReadWrite(sceneHandle, [&](physx::PxScene* scene)
			{
				PhysicsManager::GetInstance().PxRigidStaticReadWrite(mRigidHandle, [&](physx::PxRigidStatic* rigidStatic)
				{
					scene->addActor(*rigidStatic);
				});
			});
			PhysicsManager::GetInstance().PxRigidStaticReadWrite(mRigidHandle, [&](physx::PxRigidStatic* rigidStatic)
			{
				PhysicsManager::GetInstance().PxShapeReadWrite(mShapeHandle, [&](physx::PxShape* shape, physx::PxMaterial* material)
				{
					rigidStatic->attachShape(*shape);
				});
			});
		}
		else
		{
			mRigidHandle = PhysicsManager::GetInstance().CreatePxRigidDynamic(location, rotation);
			auto sceneHandle = GetGameWorld()->GetPxSceneHandle();
			PhysicsManager::GetInstance().PxSceneReadWrite(sceneHandle, [&](physx::PxScene* scene)
			{
				PhysicsManager::GetInstance().PxRigidDynamicReadWrite(mRigidHandle, [&](physx::PxRigidDynamic* rigidDynamic)
				{
					scene->addActor(*rigidDynamic);
				});
			});
			PhysicsManager::GetInstance().PxRigidDynamicReadWrite(mRigidHandle, [&](physx::PxRigidDynamic* rigidDynamic)
			{
				PhysicsManager::GetInstance().PxShapeReadWrite(mShapeHandle, [&](physx::PxShape* shape, physx::PxMaterial* material)
				{
					rigidDynamic->attachShape(*shape);
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
			auto rotation = GetOwner()->GetTransform()->GetWorldQuaternion();


			
			static physx::PxTransform transform;
			transform.p = physx::PxVec3(location.x, location.y, location.z);
			transform.q = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);
			if (mIsDynamicRigid)
			{
				PhysicsManager::GetInstance().PxRigidDynamicReadWrite(mRigidHandle, [&](physx::PxRigidDynamic* rigidDynamic)
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
			PhysicsManager::GetInstance().PxRigidDynamicReadWrite(mRigidHandle, [&](physx::PxRigidDynamic* rigidDynamic)
			{
				auto pose = rigidDynamic->getGlobalPose();
				GetOwner()->GetTransform()->SetWorldLocation(JVector3(pose.p.x, pose.p.y, pose.p.z));
				GetOwner()->GetTransform()->SetWorldQuaternion(JQuaternion(pose.q.x, pose.q.y, pose.q.z, pose.q.w));
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
				PhysicsManager::GetInstance().PxRigidDynamicReadWrite(mRigidHandle, [&](physx::PxRigidDynamic* rigidDynamic)
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
			data.Restitution = 0.8f;
			data.DynamicFriction = 0.2f;
			data.StaticFriction = 0.2f;
			data.Geometry = CreatePxGeometry();
			return data;
		});
	}
	void Collision::UpdateShape()
	{
		PhysicsManager::GetInstance().PxShapeReadWrite(mShapeHandle,
			[&](physx::PxShape* shape, physx::PxMaterial* material)
		{
			shape->setGeometry(*CreatePxGeometry());
		});
	}
	void Collision::RemoveShape()
	{
		PhysicsManager::GetInstance().PxRigidStaticReadWrite(mRigidHandle, [&](physx::PxRigidStatic* rigidStatic)
		{
			PhysicsManager::GetInstance().PxShapeReadWrite(mShapeHandle, [&](physx::PxShape* shape, physx::PxMaterial* material)
			{
				rigidStatic->detachShape(*shape);
			});
		});

		PhysicsManager::GetInstance().RemovePxShape(mShapeHandle);
		mShapeHandle = 0;
	}
	void Collision::SetCenter(const JVector3& center)
	{
		if (mCenter != center)
		{
			mIsRigidDirty = true; 
		}
		mCenter = center;

	}

	const JVector3& Collision::GetCenter() const
	{
		return mCenter;
	}

	bool Collision::IsSimulate() const
	{
		return mIsDynamicRigid;
	}

	void Collision::SetSimulate(bool isSimulate)
	{
		if (mIsDynamicRigid != isSimulate)
		{
			RemoveRigid();
			mIsDynamicRigid = isSimulate;
			CreateRigid();
		}
	}

	bool Collision::IsDebugDraw() const
	{
		return mIsDebugDraw;
	}

	void Collision::SetDebugDraw(bool isDebugDraw)
	{
		mIsDebugDraw = isDebugDraw;
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

	void SphereCollision::DrawDebugShape(physx::PxRigidActor* actor)
	{
		auto l = actor->getGlobalPose().p;

		DrawDebugSphere(JVector3(l.x, l.y, l.z), GetRadius(), GetDebugColor());
	}

	
	void SphereCollision::SetRadius(f32 r)
	{
		bool isDirty = mRadius != r;
		mRadius = r;
		if (isDirty)
		{
			UpdateShape();
		}

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
	void BoxCollision::DrawDebugShape(physx::PxRigidActor* actor)
	{

		auto l = actor->getGlobalPose().p;
		auto q = actor->getGlobalPose().q;
		
		DrawDebugBox(JVector3(l.x, l.y, l.z), JQuaternion(q.x, q.y, q.z, q.w), GetSize(), GetDebugColor());

	}
	void BoxCollision::SetSize(const JVector3& size)
	{
		bool isDirty = mSize != size;
		mSize = size;
		if (isDirty)
		{
			UpdateShape();
		}
	}
	const JVector3& BoxCollision::GetSize() const
	{
		return mSize;
	}
}