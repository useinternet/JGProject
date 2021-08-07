#include "pch.h"
#include "Transform.h"
#include "Class/Game/GameNode.h"

namespace JG
{
	void Transform::SetLocalLocation(const JVector3& location)
	{
		bool isDirty = mLocation != location;
		mLocation = location;
		if (isDirty)
		{
			SendDirty();
		}
	}
	void Transform::SetLocalRotation(const JVector3& rotation)
	{

		bool isDirty = mRotation != rotation;
		mRotation = rotation;
		CheckLimitRotation(mRotation);
		if (isDirty)
		{
			SendDirty();
		}
	}
	void Transform::SetScale(const JVector3& scale)
	{
		bool isDirty = mScale != scale;
		mScale = scale;
		mScale.x = std::max<float>(0.0000001f, scale.x);
		mScale.y = std::max<float>(0.0000001f, scale.y);
		mScale.z = std::max<float>(0.0000001f, scale.z);
		if (isDirty)
		{
			SendDirty();
		}
	}
	const JVector3& Transform::GetLocalLocation() const
	{
		return mLocation;
	}
	const JVector3& Transform::GetLocalRotation() const
	{
		return mRotation;
	}
	JVector3 Transform::GetWorldLocation() const
	{
		return mLocation;
	}
	JVector3 Transform::GetWorldRotation() const
	{
		return mRotation;
	}
	const JVector3& Transform::GetScale() const
	{
		return mScale;
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
	void Transform::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		GameComponent::MakeJson(jsonData);
		jsonData->AddMember("Location", GetLocalLocation());
		jsonData->AddMember("Rotation", GetLocalRotation());
		jsonData->AddMember("Scale", GetScale());

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
			SetScale(val->GetVector3());
		}
	}
	void Transform::UpdateWorldMatrix() const
	{
		if (mIsDirty == false)
		{
			return;
		}
		mIsDirty = false;
		mIsInvDirty = true;
		auto toRadian = Math::ConvertToRadians(mRotation);
		CheckLimitRadian(toRadian);

		mLocalMatrix = JMatrix::Scaling(mScale) * JMatrix::Rotation(JQuaternion::ToQuaternion(toRadian)) * JMatrix::Translation(mLocation);
		mWorldMatrix = mLocalMatrix;
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
	void Transform::CheckLimitRotation(JVector3& toDegree)
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
		auto scale = GetScale();



		ImGui::OnGui("Location", &location);
		ImGui::OnGui("Rotation", &rotation);
		ImGui::OnGui("Scale", &scale, padding);

		SetLocalLocation(location);
		SetLocalRotation(rotation);
		SetScale(scale);
	}
	void Transform::SendDirty()
	{
		mIsDirty = true;
		ChangeData data;
		data.Type = GetType();
		data.Object = this;
		GetOwner()->SendChangeData(data);
	}
}