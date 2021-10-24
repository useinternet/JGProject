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
		//if (mRigidHandle == 0)
		//{
		//	CreateRigid();
		//}
	
		ScheduleByFrame(0, 0, -1, 0, [&]()-> EScheduleResult
		{
			//if (mRigidHandle == 0)
			//{
			//	return EScheduleResult::Continue;
			//}
			//if (mIsDebugDraw)
			//{
			//	PhysicsManager::GetInstance().ExcuteReadCommand<physx::PxRigidActor>(
			//		mRigidHandle, [&](const physx::PxRigidActor* actor)
			//	{
			//		DrawDebugShape(actor);
			//	});
			//}
			//if (mIsDirty)
			//{
			//	PhysicsManager::GetInstance().PushWriteCommand<physx::PxRigidActor>(
			//		mRigidHandle, [&](physx::PxRigidActor* actor)
			//	{
			//		auto translate = GetOwner()->GetTransform()->GetWorldLocation() + GetCenter();
			//		auto rotation = GetOwner()->GetTransform()->GetWorldQuaternion();


			//		physx::PxTransform t;
			//		t.p = physx::PxVec3(translate.x, translate.y, translate.z);
			//		t.q = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);

			//		actor->setGlobalPose(t);
			//	});
			//	mIsDirty = false;
			//}
			//else
			//{
			//	if (IsSimulate())
			//	{
			//		PhysicsManager::GetInstance().ExcuteReadCommand<physx::PxRigidActor>(
			//			mRigidHandle, [&](const physx::PxRigidActor* actor)
			//		{
			//			auto t = actor->getGlobalPose();
			//			GetOwner()->GetTransform()->SetWorldLocation(JVector3(t.p.x, t.p.y, t.p.z));
			//			GetOwner()->GetTransform()->SetWorldQuaternion(JQuaternion(t.q.x, t.q.y, t.q.z, t.q.w));
			//		});
			//	}
			//}
			return EScheduleResult::Continue;
		});
	}

	void Collision::Destory()
	{
		
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
		//auto center = GetCenter();

		//auto label_space = ImGui::CalcTextSize("DebugDraw").x;
		//bool isSimulate = IsSimulate();
		//bool isDebugDraw = IsDebugDraw();
		//ImGui::Bool_OnGUI("Debug Draw", isDebugDraw, label_space);
		//ImGui::Bool_OnGUI("IsSimulate", isSimulate, label_space);
		//ImGui::Vector3_OnGUI("Center", center, label_space);


		//SetSimulate(isSimulate);
		//SetCenter(center);
		//SetDebugDraw(isDebugDraw);
	}

	void Collision::CreateRigid()
	{
		auto sceneHandle = GetGameWorld()->GetPxSceneHandle();
		if (mRigidHandle != 0)
		{
			PhysicsManager::GetInstance().PushDetachCommand(sceneHandle, mRigidHandle);
			PhysicsManager::GetInstance().PushRemoveCommand(mRigidHandle);
		}
		if (mShapeHandle == 0)
		{
			CreateShape();
		}


		auto translate = GetOwner()->GetTransform()->GetWorldLocation() + GetCenter();
		auto rotation = GetOwner()->GetTransform()->GetWorldQuaternion();

		if (mIsSimulate)
		{
			mRigidHandle = PhysicsManager::GetInstance().CreatePxRigidDynamic(translate, rotation);
			PhysicsManager::GetInstance().PushAttachCommand(sceneHandle, mRigidHandle);
		}
		else
		{
			mRigidHandle = PhysicsManager::GetInstance().CreatePxRigidStatic(translate, rotation);
			PhysicsManager::GetInstance().PushAttachCommand(sceneHandle, mRigidHandle);
		}


		if (mShapeHandle != 0)
		{
			PhysicsManager::GetInstance().PushAttachCommand(mRigidHandle, mShapeHandle);
		}
	}

	void Collision::CreateMaterial()
	{
		if (mMaterialHandle != 0)
		{
			PhysicsManager::GetInstance().PushRemoveCommand(mMaterialHandle);
		}

		mMaterialHandle = PhysicsManager::GetInstance().CreatePxMaterial(mStaticFriction, mDynamicFriction, mRestitution);
	}

	void Collision::CreateShape()
	{
		if (mShapeHandle != 0)
		{
			if (mRigidHandle != 0)
			{
				PhysicsManager::GetInstance().PushDetachCommand(mRigidHandle, mShapeHandle);
			}
			PhysicsManager::GetInstance().PushRemoveCommand(mShapeHandle);
		}
		if (mMaterialHandle == 0)
		{
			CreateMaterial();
		}

		mShapeHandle = PhysicsManager::GetInstance().CreatePxShape([&]() -> PxShapeData
		{
			PxShapeData data;
			data.Geometry       = CreatePxGeometry();
			data.MaterialHandle = mMaterialHandle;
			return data;
		});
	}

	void Collision::Reset()
	{
		PhysicsManager::GetInstance().PushRemoveCommand(mMaterialHandle);
		PhysicsManager::GetInstance().PushRemoveCommand(mShapeHandle);
		PhysicsManager::GetInstance().PushRemoveCommand(mRigidHandle);
	}

	void Collision::SetCenter(const JVector3& center)
	{
		bool isDirty = mCenter != center;
		mCenter = center;
		if (isDirty)
		{
			mIsDirty = true;
		}
	}

	const JVector3& Collision::GetCenter() const
	{
		return mCenter;
	}

	bool Collision::IsSimulate() const
	{
		return mIsSimulate;
	}

	void Collision::SetSimulate(bool isSimulate)
	{
		bool isDirty = mIsSimulate != isSimulate;
		mIsSimulate = isSimulate;
		if (isDirty)
		{
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

	void Collision::OnChange(const ChangeData& data)
	{
		// ∞‘¿” ∂Ù
		//if (data.Type == JGTYPE(Transform))
		//{
		//	mIsDirty = true;
		//}
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

	void SphereCollision::DrawDebugShape(const physx::PxRigidActor* actor)
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
			CreateShape();
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
	void BoxCollision::DrawDebugShape(const physx::PxRigidActor* actor)
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
			CreateShape();
		}
	}
	const JVector3& BoxCollision::GetSize() const
	{
		return mSize;
	}
}