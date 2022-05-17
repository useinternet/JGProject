#include "pch.h"
#include "Transform.h"
#include "GameNode.h"

namespace JG
{
	void Transform::SetLocalLocation(const JVector3& location)
	{
		bool mIsLocationDirty = mLocation != location;
		mLocation = location;
		if (mIsLocationDirty)
		{
			SendDirty(SendDirtyFlag_LocationDirty);
		}
	}
	void Transform::SetLocalRotation(const JVector3& rotation)
	{

		bool mIsRotationDirty = mRotation != rotation;
		mRotation = rotation;
		NormalizeRotation(mRotation);
		mQuaternion = JQuaternion::ToQuaternion(Math::ConvertToRadians(mRotation));
		if (mIsRotationDirty)
		{
			SendDirty(SendDirtyFlag_RoationDirty);
		}
	}
	void Transform::SetLocalScale(const JVector3& scale)
	{
		bool mIsScaleDirty = mScale != scale;
		mScale = scale;
		mScale.x = std::max<float>(0.0000001f, scale.x);
		mScale.y = std::max<float>(0.0000001f, scale.y);
		mScale.z = std::max<float>(0.0000001f, scale.z);
		if (mIsScaleDirty)
		{
			SendDirty(SendDirtyFlag_ScaleDirty);
		}
	}
	void Transform::SetWorldLocation(const JVector3& location)
	{
		SetLocalLocation(GetLocalLocation() + (location - GetWorldLocation()));
	}
	void Transform::SetWorldRotation(const JVector3& rotation)
	{
		SetLocalRotation(GetLocalRotation() + (rotation - GetWorldRotation()));
	}
	void Transform::SetWorldScale(const JVector3& scale)
	{
		SetLocalScale(GetLocalScale() + (scale - GetWorldScale()));
	}
	void Transform::SetLocalQuaternion(const JQuaternion& q)
	{
		bool mIsRotationDirty = mQuaternion != q;

		mQuaternion = q;
		mRotation = Math::ConvertToDegrees(JQuaternion::ToEuler(q));
		NormalizeRotation(mRotation);
		if (mIsRotationDirty)
		{
			SendDirty(SendDirtyFlag_RoationDirty);
		}
	}
	void Transform::SetWorldQuaternion(const JQuaternion& q)
	{
		auto local_q = GetLocalQuaternion() * JQuaternion::Inverse(GetWorldQuaternion()) * q;
		SetLocalQuaternion(local_q);
	}
	const JVector3& Transform::GetLocalLocation() const
	{
		return mLocation;
	}
	const JVector3& Transform::GetLocalRotation() const
	{
		return mRotation;
	}
	const JVector3& Transform::GetLocalScale() const
	{
		return mScale;
	}
	const JVector3& Transform::GetWorldLocation() const
	{
		UpdateWorldLocation();
		return mWorldLocation;
	}
	const JVector3& Transform::GetWorldRotation() const
	{
		UpdateWorldRotation();
		return mWorldRotation;
	}
	const JVector3& Transform::GetWorldScale() const
	{
		UpdateWorldScale();
		return mWorldScale;
	}

	const JMatrix& Transform::GetWorldMatrix() const
	{
		UpdateWorldMatrix();
		return mWorldMatrix;
	}
	const JMatrix& Transform::GetInvWorldMatrix() const
	{
		UpdateInvWorldMatrix();
		return mInvWorldMatrix;
	}

	const JQuaternion& Transform::GetLocalQuaternion() const
	{
		return mQuaternion;
	}
	const JQuaternion& Transform::GetWorldQuaternion() const
	{
		UpdateWorldRotation();
		return mWorldQuaternion;
	}
	const JVector3& Transform::GetLook() const
	{
		UpdateWorldRotation();
		return mLookVec;
	}
	const JVector3& Transform::GetRight() const
	{
		UpdateWorldRotation();
		return mRightVec;
	}
	const JVector3& Transform::GetUp() const
	{
		UpdateWorldRotation();
		return mUpVec;
	}
	void Transform::NormalizeRotation(JVector3& toDegree) const
	{
		while (toDegree.x <= 0.0f)
		{
			toDegree.x += 360.0f;
		}
		while (toDegree.x >= 360.0f)
		{
			toDegree.x -= 360.0f;
		}
		while (toDegree.y <= 0.0f)
		{
			toDegree.y += 360.0f;
		}
		while (toDegree.y >= 360.0f)
		{
			toDegree.y -= 360.0f;
		}
		while (toDegree.z <= 0.0f)
		{
			toDegree.z += 360.0f;
		}
		while (toDegree.z >= 360.0f)
		{
			toDegree.z -= 360.0f;
		}
	}
	void Transform::Refresh()
	{
		SendDirty(SendDirtyFlag_All);
	}
	void Transform::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		GameComponent::MakeJson(jsonData);
		jsonData->AddMember("Location", GetLocalLocation());
		jsonData->AddMember("Rotation", GetLocalRotation());
		jsonData->AddMember("Scale", GetLocalScale());



	}
	void Transform::LoadJson(SharedPtr<JsonData> jsonData)
	{
		GameComponent::LoadJson(jsonData);

		auto val = jsonData->GetMember("Location");
		if (val)
		{
			SetLocalLocation(val->GetVector3());
		}
		val = jsonData->GetMember("Rotation");
		if (val)
		{
			SetLocalRotation(val->GetVector3());
		}
		val = jsonData->GetMember("Scale");
		if (val)
		{
			SetLocalScale(val->GetVector3());
		}
	}
	void Transform::UpdateWorldMatrix() const
	{
		if (mIsDirty == false)
		{
			return;
		}
		mIsDirty    = false;
		mIsInvDirty = true;

		auto worldLocation = GetWorldLocation();
		auto worldQuat     = GetWorldQuaternion();
		auto worldScale    = GetWorldScale();
		mWorldMatrix = JMatrix::Scaling(worldScale) * JMatrix::Rotation(worldQuat) * JMatrix::Translation(worldLocation);

	}
	void Transform::UpdateWorldLocation() const
	{
		if (mIsLocationDirty == false || GetOwner() == nullptr)
		{
			return;
		}
		mIsLocationDirty = false;

		auto parent = GetOwner()->GetParent();
		if (parent == nullptr)
		{
			mWorldLocation = GetLocalLocation();
		}
		else
		{
			auto pwp = parent->GetTransform()->GetWorldLocation();
			auto local = GetLocalLocation();
			mWorldLocation = GetLocalLocation() + parent->GetTransform()->GetWorldLocation();
		}
	}
	void Transform::UpdateWorldRotation() const
	{
		if (mIsRotationDirty == false || GetOwner() == nullptr)
		{
			return;
		}
		mIsRotationDirty = false;
		auto parent = GetOwner()->GetParent();
		if (parent == nullptr)
		{
			mWorldRotation   = GetLocalRotation();
			mWorldQuaternion = GetLocalQuaternion();
		}
		else
		{
			mWorldRotation   = GetLocalRotation() + parent->GetTransform()->GetWorldRotation();
			mWorldQuaternion = parent->GetTransform()->GetWorldQuaternion() * GetLocalQuaternion();
		}
		NormalizeRotation(mWorldRotation);
		/*
				auto rotation = GetOwner()->GetTransform()->GetWorldRotation();
		rotation = Math::ConvertToRadians(rotation);
		return JVector3::Normalize(JMatrix::Rotation(rotation).TransformVector(JVector3(0, 0, 1)));
		*/

		// Look, Up, Right Update
		mLookVec  = JMatrix::Rotation(mWorldQuaternion).TransformVector(JVector3::FowardVector());
		mRightVec = JMatrix::Rotation(mWorldQuaternion).TransformVector(JVector3::RightVector());
		mUpVec	  = JMatrix::Rotation(mWorldQuaternion).TransformVector(JVector3::UpVector());

		mLookVec  = JVector3::Normalize(mLookVec);
		mRightVec = JVector3::Normalize(mRightVec);
		mUpVec	  = JVector3::Normalize(mUpVec);
	}
	void Transform::UpdateWorldScale() const
	{
		if (mIsScaleDirty == false || GetOwner() == nullptr)
		{
			return;
		}
		mIsScaleDirty = false;
		auto parent = GetOwner()->GetParent();
		if (parent == nullptr)
		{
			mWorldScale = GetLocalScale();
		}
		else
		{
			auto localScale = GetLocalScale();
			auto p_worldScale = parent->GetTransform()->GetWorldScale();
			mWorldScale = JVector3(localScale.x * p_worldScale.x, localScale.y * p_worldScale.y, localScale.z * p_worldScale.z);
		}
	}
	void Transform::UpdateInvWorldMatrix() const
	{
		if (mIsInvDirty == false)
		{
			return;
		}
		mIsInvDirty = false;
		mInvWorldMatrix = JMatrix::Inverse(GetWorldMatrix());
	}

	void Transform::OnChange(const ChangeData& data)
	{
		if (data.Type == JGTYPE(Transform))
		{
			ApplyDirtyFlag((SendDirtyFlags)data.UserMsg);
		}
	}
	void Transform::SendDirty(SendDirtyFlags flags)
	{
		ApplyDirtyFlag(flags);
		ChangeData data;
		data.Type   = GetType();
		data.Object = this;
		data.UserMsg = flags;
		
		if (GetOwner() != nullptr)
		{
			GetOwner()->SendChangeData(data);
		}

	}
	void Transform::ApplyDirtyFlag(SendDirtyFlags flags)
	{
		if (flags & SendDirtyFlag_LocationDirty)
		{
			mIsLocationDirty = true;
		}
		if (flags & SendDirtyFlag_RoationDirty)
		{
			mIsRotationDirty = true;
		}
		if (flags & SendDirtyFlag_ScaleDirty)
		{
			mIsScaleDirty = true;
		}
		mIsDirty = true;
	}
}