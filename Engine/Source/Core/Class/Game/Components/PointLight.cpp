#include "pch.h"
#include "PointLight.h"
#include "ExternalImpl/ImGuiExternal.h"
#include "Transform.h"
#include "Class/Game/GameNode.h"
#include "Class/Game/Components/Camera.h"
#include "Class/Game/GameWorld.h"
namespace JG
{
	void PointLight::Awake()
	{
#ifdef JG_EDITOR
		mIcon = GetGameWorld()->GetAssetManager()->RequestOriginAsset<ITexture>("Asset/Engine/Icon/Icon_Light.jgasset");
		mPushDebugHandle = Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::Graphics_PushSceneObject, SCHEDULE_BIND_FN(&PointLight::PushDebugRenderItem));
#endif
		mPushLightScheduleHandle = Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::Graphics_PushSceneObject, SCHEDULE_BIND_FN(&PointLight::PushLightItem));
	}
	void PointLight::Start()
	{
		Light::Start();


	}
	void PointLight::Destory()
	{
		Light::Destory();
		if (mPushLightScheduleHandle != nullptr)
		{
			mPushLightScheduleHandle->Reset();
			mPushLightScheduleHandle = nullptr;
		}
#ifdef JG_EDITOR
		if (mPushDebugHandle != nullptr)
		{
			mPushDebugHandle->Reset();
			mPushDebugHandle = nullptr;
		}
#endif
	}
	void PointLight::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		Light::MakeJson(jsonData);

		jsonData->AddMember("Color", mColor);
		jsonData->AddMember("Intensity", mIntensity);
		jsonData->AddMember("Range", mRange);
		jsonData->AddMember("Att0", mAtt0);
		jsonData->AddMember("Att1", mAtt1);
		jsonData->AddMember("Att2", mAtt2);


	}
	void PointLight::LoadJson(SharedPtr<JsonData> jsonData)
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
		val = jsonData->GetMember("Range");
		if (val)
		{
			mRange = val->GetFloat();
		}
		val = jsonData->GetMember("Att0");
		if (val)
		{
			mAtt0 = val->GetFloat();
		}
		val = jsonData->GetMember("Att1");
		if (val)
		{
			mAtt1 = val->GetFloat();
		}
		val = jsonData->GetMember("Att2");
		if (val)
		{
			mAtt2 = val->GetFloat();
		}
	}
	void PointLight::SetColor(const Color& color)
	{
		mColor = JVector3(color.R, color.G, color.B);
	}
	void PointLight::SetIntensity(f32 intensity)
	{
		mIntensity = intensity;
	}
	void PointLight::SetRange(f32 range)
	{
		mRange = range;
	}
	void PointLight::SetAtt0(f32 att0)
	{
		mAtt0 = att0;
	}
	void PointLight::SetAtt1(f32 att1)
	{
		mAtt1 = att1;
	}
	void PointLight::SetAtt2(f32 att2) 
	{
		mAtt2 = att2;
	}

	Color PointLight::GetColor() const
	{
		return Color(mColor.x, mColor.y, mColor.z, 1.0f);
	}
	f32 PointLight::GetIntensity() const
	{
		return mIntensity;
	}
	f32 PointLight::GetRange() const
	{
		return mRange;
	}
	f32 PointLight::GetAtt0() const
	{
		return mAtt0;
	}
	f32 PointLight::GetAtt1() const
	{
		return mAtt1;
	}
	f32 PointLight::GetAtt2() const
	{
		return mAtt2;
	}
#ifdef JG_EDITOR
	EScheduleResult PointLight::PushDebugRenderItem()
	{
		auto debugObject = CreateSharedPtr<Graphics::PaperObject>();


		auto mainCam = Camera::GetMainCamera();
		if (mainCam == nullptr)
		{
			return EScheduleResult::Continue;
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
		f32 scale = dis / 10;
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


		debugObject->WorldMatrix = billboardMatrix;
		if (mIcon && mIcon->Get() && mIcon->Get()->IsValid())
		{
			debugObject->Texture = mIcon->Get();
		}

		GetGameWorld()->PushRenderSceneObject(debugObject);
		return EScheduleResult::Continue;
	}
#endif
	EScheduleResult PointLight::PushLightItem()
	{
		if (IsActive() == false)
		{
			return EScheduleResult::Continue;
		}
		auto lightObject = CreateSharedPtr<Graphics::PointLight>();
		lightObject->Color = mColor;
		lightObject->Intensity = mIntensity;
		lightObject->Position = GetOwner()->GetTransform()->GetWorldLocation();
		lightObject->Range = mRange;
		lightObject->Att0 = mAtt0;
		lightObject->Att1 = mAtt1;
		lightObject->Att2 = mAtt2;
		GetGameWorld()->PushRenderLightObject(lightObject);

		return EScheduleResult::Continue;
	}

	void PointLight::OnInspectorGUI()
	{
		Light::OnInspectorGUI();
		ImGui::Spacing();

		Color color   = GetColor();
		f32 intensity = GetIntensity();
		f32 range     = GetRange();
		f32 att0      = GetAtt0();
		f32 att1      = GetAtt1();
		f32 att2      = GetAtt2();



		f32 label_width = ImGui::CalcTextSize("Intensity").x;
		ImGui::Color3_OnGUI("Color", color, label_width);
		ImGui::Float_OnGUI("Intensity", intensity,  label_width);
		ImGui::Float_OnGUI("Range", range, label_width);
		ImGui::Float_Slider_OnGUI("Att0", att0, 0.0f, 1.0f, label_width);
		ImGui::Float_Slider_OnGUI("Att1", att1, 0.0f, 1.0f, label_width);
		ImGui::Float_Slider_OnGUI("Att2", att2, 0.0f, 1.0f, label_width);

		SetColor(Color(color.R, color.G, color.B, 1.0f));
		SetIntensity(intensity);
		SetRange(range);
		SetAtt0(att0);
		SetAtt1(att1);
		SetAtt2(att2);
	}

}