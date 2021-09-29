#include "pch.h"
#include "PointLight.h"
#include "ExternalImpl/ImGuiExternal.h"
#include "Transform.h"
#include "Class/Game/GameNode.h"
#include "Class/Game/Components/Camera.h"
#include "Class/Game/GameWorld.h"
namespace JG
{
	void PointLight::Start()
	{
		Light::Start();
		if (mRI == nullptr)
		{
			mRI = CreateSharedPtr<PointLightItem>();

		}


#ifdef JG_EDITOR
		if (mTargetRI == nullptr)
		{
			mTargetRI = CreateSharedPtr<Standard2DRenderItem>();
			mTargetRI->Color = Color::White();
			mIcon = GetGameWorld()->GetAssetManager()->RequestOriginAsset<ITexture>("Asset/Engine/Icon/Icon_Light.jgasset");
		}
#endif


	}
	void PointLight::Destory()
	{
		Light::Destory();



	}
	void PointLight::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		Light::MakeJson(jsonData);

		jsonData->AddMember("Color", mRI->Color);
		jsonData->AddMember("Intensity", mRI->Intensity);
		jsonData->AddMember("Range", mRI->Range);
		jsonData->AddMember("Att0", mRI->Att0);
		jsonData->AddMember("Att1", mRI->Att1);
		jsonData->AddMember("Att2", mRI->Att2);


	}
	void PointLight::LoadJson(SharedPtr<JsonData> jsonData)
	{
		Light::LoadJson(jsonData);
		mRI = CreateSharedPtr<PointLightItem>();
		auto val = jsonData->GetMember("Color");
		if (val)
		{
			mRI->Color = val->GetVector3();
		}
		val = jsonData->GetMember("Intensity");
		if (val)
		{
			mRI->Intensity = val->GetFloat();
		}
		val = jsonData->GetMember("Range");
		if (val)
		{
			mRI->Range = val->GetFloat();
		}
		val = jsonData->GetMember("Att0");
		if (val)
		{
			mRI->Att0 = val->GetFloat();
		}
		val = jsonData->GetMember("Att1");
		if (val)
		{
			mRI->Att1 = val->GetFloat();
		}
		val = jsonData->GetMember("Att2");
		if (val)
		{
			mRI->Att2 = val->GetFloat();
		}
	}
#ifdef JG_EDITOR
	SharedPtr<IRenderItem> PointLight::PushDebugRenderItem()
	{

		if (mTargetRI != nullptr)
		{
			auto mainCam = Camera::GetMainCamera();
			if (mainCam == nullptr)
			{
				return nullptr;
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

				auto localRay  = pickingRay;
				auto& invWorld = JMatrix::Inverse(billBoardWorldMat);
				localRay.dir = invWorld.TransformVector(localRay.dir);
				localRay.origin = invWorld.TransformPoint(localRay.origin);

				return bbox->Intersection(localRay);
			}, nullptr, &billboardMatrix, sizeof(JMatrix));


			mTargetRI->WorldMatrix = billboardMatrix;
			if (mIcon && mIcon->Get() && mIcon->Get()->IsValid())
			{
				mTargetRI->Texture = mIcon->Get();
			}
		}


		return mTargetRI;
	}
#endif
	SharedPtr<ILightItem> PointLight::PushLightItem()
	{
		if (mRI != nullptr)
		{
			mRI->Position = GetOwner()->GetTransform()->GetWorldLocation();
		}



		return mRI;
	}

	void PointLight::OnInspectorGUI()
	{
		Light::OnInspectorGUI();
		ImGui::Spacing();

		Color color = Color(mRI->Color.x, mRI->Color.y, mRI->Color.z, 1.0f);
		f32 intensity = mRI->Intensity;
		f32 range = mRI->Range;
		f32 att0 = mRI->Att0;
		f32 att1 = mRI->Att1;
		f32 att2 = mRI->Att2;



		f32 label_width = ImGui::CalcTextSize("Intensity").x;
		ImGui::Color3_OnGUI("Color", color, label_width);
		ImGui::Float_OnGUI("Intensity", intensity,  label_width);
		ImGui::Float_OnGUI("Range", range, label_width);
		ImGui::Float_Slider_OnGUI("Att0", att0, 0.0f, 1.0f, label_width);
		ImGui::Float_Slider_OnGUI("Att1", att1, 0.0f, 1.0f, label_width);
		ImGui::Float_Slider_OnGUI("Att2", att2, 0.0f, 1.0f, label_width);

		mRI->Color = JVector3(color.R, color.G, color.B);
		mRI->Intensity = intensity;
		mRI->Range = range;
		mRI->Att0 = att0;
		mRI->Att1 = att1;
		mRI->Att2 = att2;
	}

}