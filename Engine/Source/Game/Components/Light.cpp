#include "pch.h"
#include "Light.h"
#include "Transform.h"
#include "GameNode.h"
#include "Components/Camera.h"
#include "GameWorld.h"

namespace JG
{
	void Light::Awake()
	{
#ifdef JG_EDITOR
		mIcon = GetGameWorld()->GetAssetManager()->RequestOriginAsset<ITexture>("Asset/Engine/Icon/Icon_Light.jgasset");
#endif
	}
	void Light::Start()
	{
#ifdef JG_EDITOR
		mIsEditorMode = false;
#endif
	}
	void Light::Update()
	{
	}
	void Light::FixedUpdate()
	{
#ifdef JG_EDITOR
		PushDebugRenderItem();
#endif
		if (IsActive() == true)
		{
			PushLightItem();
		}
	}
	void Light::Destory()
	{
	}
	void Light::MakeJson(SharedPtr<JsonData> jsonData) const
	{
	}
	void Light::LoadJson(SharedPtr<JsonData> jsonData)
	{
	}
	void Light::CalcAtt(f32 attRange, f32* att0, f32* att1, f32* att2)
	{
		JVector3 att;
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
			*att0 = att.y;
			*att1 = att.z;
			*att2 = att.w;
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
			*att0 = Att0;
			*att1 = Att1;
			*att2 = Att2;
		}
	}
	void Light::PushLightItem()
	{
	}
	void Light::PushDebugRenderItem()
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
}