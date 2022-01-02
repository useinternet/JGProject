#include "pch.h"
#include "PointLight.h"
#include "Transform.h"
#include "GameNode.h"
#include "Components/Camera.h"
#include "GameWorld.h"
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
#ifdef JG_EDITOR
		mIsEditorMode = false;
#endif
	}
	void PointLight::Update()
	{
		Light::Update();
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
		jsonData->AddMember("AttRange", mAttRange);
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
		val = jsonData->GetMember("AttRange");
		if (val)
		{
			SetAttRange(val->GetFloat());
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
	void PointLight::SetAttRange(f32 attRange, bool auto_calc_att)
	{
		mAttRange = Math::Min(attRange, GetRange());
		if (auto_calc_att)
		{
			CalcAtt();
		}

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
	f32 PointLight::GetAttRange() const
	{
		return mAttRange;
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
	void PointLight::CalcAtt()
	{
		static List<JVector4> RecommendAttValues;
		static const u64 RecommandAttCount = 12;
		if (RecommendAttValues.empty())
		{
			RecommendAttValues.resize(RecommandAttCount);
			RecommendAttValues[0] = JVector4(7, 1, 0.7f, 1.8f);
			RecommendAttValues[1] = JVector4(13, 1, 0.35f, 0.44f);
			RecommendAttValues[2] = JVector4(20, 1, 0.22f, 0.20f);
			RecommendAttValues[3] = JVector4(32, 1, 0.14f, 0.07f);
			RecommendAttValues[4] = JVector4(50, 1, 0.09f, 0.032f);
			RecommendAttValues[5] = JVector4(65, 1, 0.07f, 0.017f);
			RecommendAttValues[6] = JVector4(100, 1, 0.045f, 0.0075f);
			RecommendAttValues[7] = JVector4(160, 1, 0.027f, 0.0025f);
			RecommendAttValues[8] = JVector4(200, 1, 0.022f, 0.0019f);
			RecommendAttValues[9] = JVector4(325, 1, 0.014f, 0.0007f);
			RecommendAttValues[10] = JVector4(600, 1, 0.007f, 0.0002f);
			RecommendAttValues[11] = JVector4(3250, 1, 0.0014f, 0.000007f);
		}
		f32 attRange = GetAttRange();
		i32 recommandIndex = 0;
		for (i32 i = 0; i < 12; ++i)
		{
			f32 d = RecommendAttValues[i].x;
			if (d >= attRange)
			{
				recommandIndex = Math::Max(i - 1, 0);
				
				break;
			}
		}

		if (recommandIndex >= RecommandAttCount - 1)
		{
			JVector4 att = RecommendAttValues[recommandIndex];
			SetAtt0(att.y);
			SetAtt1(att.z);
			SetAtt2(att.w);
		}
		else
		{
			// 1
			JVector4 A = RecommendAttValues[recommandIndex];
			JVector4 B = RecommendAttValues[recommandIndex + 1];
			f32 Alpha = ((B.x - A.x) - (attRange - A.x)) / (B.x - A.x);
			f32 Att0 = Math::Lerp(A.y, B.y, Alpha);
			f32 Att1 = Math::Lerp(A.z, B.z, Alpha);
			f32 Att2 = Math::Lerp(A.w, B.w, Alpha);
			SetAtt0(Att0);
			SetAtt1(Att1);
			SetAtt2(Att2);
		}
	}
#ifdef JG_EDITOR
	EScheduleResult PointLight::PushDebugRenderItem()
	{
		if (mIsEditorMode == false)
		{
			return EScheduleResult::Continue;
		}
		//auto debugObject = CreateSharedPtr<Graphics::PaperObject>();
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

		//debugObject->WorldMatrix = billboardMatrix;
		if (mIcon && mIcon->Get() && mIcon->Get()->IsValid())
		{
			texture = mIcon->Get();
			//debugObject->Texture = mIcon->Get();
		}

		RequestDrawEditorUIInSceneView e;
		e.Data.WorldMatrix = billboardMatrix;
		e.Data.Texture = texture;
		//GetGameWorld()->PushRenderSceneObject(debugObject);
		SendEvent(e);
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
		lightObject->AttRange = mAttRange;
		lightObject->Att0 = mAtt0;
		lightObject->Att1 = mAtt1;
		lightObject->Att2 = mAtt2;
		GetGameWorld()->PushRenderLightObject(lightObject);

		return EScheduleResult::Continue;
	}
}