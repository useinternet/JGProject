#include "pch.h"
#include "Transform.h"
#include "Class/Game/GameNode.h"

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
		CheckLimitRotation(mRotation);
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
		auto worldRotation = GetWorldRotation();
		auto worldScale    = GetWorldScale();
		auto toRadian      = Math::ConvertToRadians(worldRotation);
		CheckLimitRadian(toRadian);

		mWorldMatrix = JMatrix::Scaling(worldScale) * JMatrix::Rotation(JQuaternion::ToQuaternion(toRadian)) * JMatrix::Translation(worldLocation);
	}
	void Transform::UpdateWorldLocation() const
	{
		if (mIsLocationDirty == false)
		{
			return;
		}
		if (GetOwner()->GetName() == "Test")
		{
			JG_CORE_INFO("Update WorldLocation : {0}", GetOwner()->GetName());
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
		if (mIsRotationDirty == false)
		{
			return;
		}
		mIsRotationDirty = false;
		auto parent = GetOwner()->GetParent();
		if (parent == nullptr)
		{
			mWorldRotation = GetLocalRotation();
		}
		else
		{
			mWorldRotation = GetLocalRotation() + parent->GetTransform()->GetWorldRotation();
		}

	}
	void Transform::UpdateWorldScale() const
	{
		if (mIsScaleDirty == false)
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
	void Transform::CheckLimitRadian(JVector3& toRadian) const
	{
		while (toRadian.x <= 0.0f)
		{
			toRadian.x += JG_2PI;
		}
		while (toRadian.x >= JG_2PI)
		{
			toRadian.x -= JG_2PI;
		}
		while (toRadian.y <= 0.0f)
		{
			toRadian.y += JG_2PI;
		}
		while (toRadian.y >= JG_2PI)
		{
			toRadian.y -= JG_2PI;
		}
		while (toRadian.z <= 0.0f)
		{
			toRadian.z += JG_2PI;
		}
		while (toRadian.z >= JG_2PI)
		{
			toRadian.z -= JG_2PI;
		}
	}
	void Transform::CheckLimitRotation(JVector3& toDegree) const
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
	void Transform::OnInspectorGUI()
	{
		float padding = ImGui::CalcTextSize("Location").x - ImGui::CalcTextSize("Scale").x;
		
		auto location = GetLocalLocation();
		auto rotation = GetLocalRotation();
		auto scale = GetLocalScale();



		ImGui::OnGui("Location", &location);
		ImGui::OnGui("Rotation", &rotation);
		ImGui::OnGui("Scale", &scale, padding);

		SetLocalLocation(location);
		SetLocalRotation(rotation);
		SetLocalScale(scale);
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
		
		GetOwner()->SendChangeData(data);
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