#include "pch.h"
#include "SpotLight.h"
#include "Transform.h"
#include "GameNode.h"
#include "Components/Camera.h"
#include "GameWorld.h"
namespace JG
{
	void SpotLight::Awake()
	{
		Light::Awake();


	}
	void SpotLight::Start()
	{
		Light::Start();
	}
	void SpotLight::Update()
	{
		Light::Update();
	}
	void SpotLight::FixedUpdate()
	{
		Light::FixedUpdate();
	}
	void SpotLight::Destory()
	{
		Light::Destory();
	}
	void SpotLight::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		Light::MakeJson(jsonData);
	}
	void SpotLight::LoadJson(SharedPtr<JsonData> jsonData)
	{
		Light::LoadJson(jsonData);
	}
	void SpotLight::SetRange(f32 range)
	{
		mRange = range;
	}
	f32 SpotLight::GetRange() const
	{
		return mRange;
	}
	void SpotLight::SetOutFallOff(f32 angle)
	{
		mOutFallOff = angle;
	}
	f32 SpotLight::GetOutFallOff() const
	{
		return mOutFallOff;
	}
	void SpotLight::SetFallOff(f32 angle)
	{
		mFallOff = Math::Min(angle, mOutFallOff);
	}
	f32 SpotLight::GetFallOff() const
	{
		return mFallOff;
	}
	void SpotLight::SetIntensity(f32 intensity)
	{
		mIntensity = intensity;
	}
	f32 SpotLight::GetIntensity() const
	{
		return mIntensity;
	}
	void SpotLight::SetColor(const Color& color)
	{
		mColor = color;
	}
	Color SpotLight::GetColor() const
	{
		return Color(mColor.x, mColor.y, mColor.z, 1.0f);
	}
	void SpotLight::SetAttRange(f32 attRange, bool auto_calc_att)
	{
		mAttRange = Math::Min(attRange, GetRange());
		if (auto_calc_att)
		{
			CalcAtt(attRange, &mAtt0, &mAtt1, &mAtt2);
		}
	}
	f32 SpotLight::GetAttRange() const
	{
		return mAttRange;
	}
	void SpotLight::SetAtt0(f32 att0)
	{
		mAtt0 = att0;
	}
	f32 SpotLight::GetAtt0() const
	{
		return mAtt0;
	}
	void SpotLight::SetAtt1(f32 att1)
	{
		mAtt1 = att1;
	}
	f32 SpotLight::GetAtt1() const
	{
		return mAtt1;
	}
	void SpotLight::SetAtt2(f32 att2)
	{
		mAtt2 = att2;
	}
	f32 SpotLight::GetAtt2() const
	{
		return mAtt2;
	}
	const JVector3& SpotLight::GetDirection() const
	{
		return mDirection;
	}

	void SpotLight::PushLightItem()
	{
		auto lightObject = CreateSharedPtr<Graphics::SpotLight>();
		auto transform = GetOwner()->GetTransform();
		JQuaternion q = transform->GetWorldQuaternion();
		JVector3 look = JMatrix::Rotation(q).TransformVector(JVector3(0, 0, 1));
		mDirection = JVector3::Normalize(look);


		lightObject->Position = transform->GetWorldLocation();
		lightObject->Direction = mDirection;
		lightObject->Color = mColor;
		lightObject->Intensity = mIntensity;
		lightObject->Range = mRange;
		lightObject->OutFallOff = std::cos(Math::ConvertToRadians(mOutFallOff));
		lightObject->FallOff    = std::cos(Math::ConvertToRadians(mFallOff));
		lightObject->AttRnage = mAttRange;
		lightObject->Att0 = mAtt0;
		lightObject->Att1 = mAtt1;
		lightObject->Att2 = mAtt2;
		GetGameWorld()->PushRenderLightObject(lightObject);
	}
}