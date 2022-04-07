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
#ifdef JG_EDITOR
		mIcon = GetGameWorld()->GetAssetManager()->RequestOriginAsset<ITexture>("Asset/Engine/Icon/Icon_Light.jgasset");
#endif
    }
    void DirectionalLight::Start()
    {
		Light::Start();
#ifdef JG_EDITOR
		mIsEditorMode = false;
#endif
    }
    void DirectionalLight::Update()
    {
		Light::Update();
    }
	void DirectionalLight::FixedUpdate()
	{
		Light::FixedUpdate();
#ifdef JG_EDITOR
		PushDebugRenderItem();
#endif
		PushLightItem();
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
		jsonData->AddMember("Distance", mDistance);
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
		val = jsonData->GetMember("Distance");
		if (val)
		{
			mDistance = val->GetFloat();
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
	void DirectionalLight::SetDistance(f32 distance)
	{
		mDistance = distance;
	}
	f32 DirectionalLight::GetDistance() const
	{
		return mDistance;
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
    void DirectionalLight::PushDebugRenderItem()
    {
		if (mIsEditorMode == false)
		{
			return;
		}
		auto mainCam = Camera::GetMainCamera();
		if (mainCam == nullptr)
		{
			return;
		}


		auto camPos = mainCam->GetOwner()->GetTransform()->GetWorldLocation();
		auto location = GetOwner()->GetTransform()->GetWorldLocation();


		auto v = camPos - location;
		auto viewMatrix = mainCam->GetViewMatrix();

		viewMatrix.Get(0, 3) = 0.0f;
		viewMatrix.Get(1, 3) = 0.0f;
		viewMatrix.Get(2, 3) = 0.0f;
		viewMatrix = JMatrix::Inverse(viewMatrix);




		viewMatrix.Get(0, 3) = location.x;
		viewMatrix.Get(1, 3) = location.y;
		viewMatrix.Get(2, 3) = location.z;


		f32 dis = JVector3::Length(location - mainCam->GetOwner()->GetTransform()->GetWorldLocation());
		f32 scale = dis / 15;
		auto billboardMatrix = JMatrix::Scaling(JVector3(scale, scale, 1.0f)) * viewMatrix;
		//
		JBBox bbox;
		bbox.min = JVector3(-0.5f, -0.5f, 0.0f);
		bbox.max = JVector3(0.5f, 0.5f, 0.0f);

		GetOwner()->SetPickingBoundingBox(bbox,
			[&](const JBBox* bbox, const JRay& pickingRay, void* userData) -> bool
		{
			JMatrix billBoardWorldMat = *((JMatrix*)userData);

			auto localRay = pickingRay;
			auto& invWorld = JMatrix::Inverse(billBoardWorldMat);
			localRay.dir = invWorld.TransformVector(localRay.dir);
			localRay.origin = invWorld.TransformPoint(localRay.origin);

			return bbox->Intersection(localRay);
		}, nullptr, &billboardMatrix, sizeof(JMatrix));


		SharedPtr<ITexture> texture = nullptr;
		if (mIcon && mIcon->Get() && mIcon->Get()->IsValid())
		{
			texture = mIcon->Get();
		}

		RequestDrawEditorUIInSceneView e;
		e.Data.WorldMatrix = billboardMatrix;
		e.Data.Texture = texture;

		SendEvent(e);
    }
    void DirectionalLight::PushLightItem()
    {
		if (IsActive() == false)
		{
			return;
		}
		auto lightObject = CreateSharedPtr<Graphics::DirectionalLight>();

		JQuaternion q = GetOwner()->GetTransform()->GetWorldQuaternion();
		JVector3 look = JMatrix::Rotation(q).TransformVector(JVector3(0, 0, 1));
		mDirection = JVector3::Normalize(look);

		lightObject->Color = mColor;
		lightObject->Direction = mDirection;
		lightObject->Distance = mDistance;
		lightObject->Intensity = mIntensity;
		GetGameWorld()->PushRenderLightObject(lightObject);
    }
}