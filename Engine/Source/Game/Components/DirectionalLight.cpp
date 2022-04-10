#include "pch.h"
#include "DirectionalLight.h"
#include "Transform.h"
#include "GameNode.h"
#include "Components/Camera.h"
#include "GameWorld.h"

namespace JG
{
    void DirectionalLight::Awake()
    {
		Light::Awake();
    }
    void DirectionalLight::Start()
    {
		Light::Start();

    }
    void DirectionalLight::Update()
    {
		Light::Update();
    }
	void DirectionalLight::FixedUpdate()
	{
		Light::FixedUpdate();
	}
    void DirectionalLight::Destory()
    {
		Light::Destory();
    }
    void DirectionalLight::MakeJson(SharedPtr<JsonData> jsonData) const
    {
		Light::MakeJson(jsonData);
		jsonData->AddMember("Color", mColor);
		jsonData->AddMember("Intensity", mIntensity);
    }
    void DirectionalLight::LoadJson(SharedPtr<JsonData> jsonData)
    {
		Light::LoadJson(jsonData);
		auto val = jsonData->GetMember("Color");
		if (val)
		{
			mColor = val->GetVector3();
		}
		val = jsonData->GetMember("Intensity");
		if (val)
		{
			mIntensity = val->GetFloat();
		}
    }
	void DirectionalLight::SetIntensity(f32 intensity)
	{
		mIntensity = intensity;
	}
	f32 DirectionalLight::GetIntensity() const
	{
		return mIntensity;
	}
	void DirectionalLight::SetColor(const Color& color)
	{
		mColor = JVector3(color.R, color.G, color.B);
	}
	const JVector3& DirectionalLight::GetDirection() const
	{
		return mDirection;
	}
	Color DirectionalLight::GetColor() const
    {
        return Color(mColor.x, mColor.y, mColor.z, 1.0f);
    }

    void DirectionalLight::PushLightItem()
    {
		auto lightObject = CreateSharedPtr<Graphics::DirectionalLight>();

		JQuaternion q = GetOwner()->GetTransform()->GetWorldQuaternion();
		JVector3 look = JMatrix::Rotation(q).TransformVector(JVector3(0, 0, 1));
		mDirection = JVector3::Normalize(look);

		lightObject->Color = mColor;
		lightObject->Direction = mDirection;
		lightObject->Intensity = mIntensity;
		GetGameWorld()->PushRenderLightObject(lightObject);
    }
}