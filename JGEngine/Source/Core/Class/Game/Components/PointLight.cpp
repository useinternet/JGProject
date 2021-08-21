#include "pch.h"
#include "PointLight.h"
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
			mIcon = GetGameWorld()->GetAssetManager()->RequestOriginAsset<ITexture>("Asset/Engine/Icon/Light_Icon.jgasset");
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
	SharedPtr<IRenderItem> PointLight::PushRenderItem()
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

			auto billboardMatrix = JMatrix::Scaling(JVector3(1.5f, 1.5f, 1.0f)) * viewMatrix;
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


		ImGui::ColorEdit3("Color", (float*)&(mRI->Color));


	
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Intensity "); ImGui::SameLine();
		ImGui::SetNextItemWidth(150.0f);
		ImGui::InputFloat("##Intensity", &mRI->Intensity);


		ImGui::AlignTextToFramePadding();
		ImGui::Text("Range    "); ImGui::SameLine();
		ImGui::SetNextItemWidth(150.0f);
		ImGui::InputFloat("##Range", &mRI->Range);


		ImGui::AlignTextToFramePadding();
		ImGui::Text("Att0     "); ImGui::SameLine();
		ImGui::SetNextItemWidth(150.0f);
		ImGui::SliderFloat("##Att0", &mRI->Att0, 0.0f , 1.0f);


		ImGui::AlignTextToFramePadding();
		ImGui::Text("Att1     "); ImGui::SameLine();
		ImGui::SetNextItemWidth(150.0f);
		ImGui::SliderFloat("##Att1", &mRI->Att1, 0.0f, 1.0f);


		ImGui::AlignTextToFramePadding();
		ImGui::Text("Att2     "); ImGui::SameLine();
		ImGui::SetNextItemWidth(150.0f);
		ImGui::SliderFloat("##Att2", &mRI->Att2, 0.0f, 1.0f);

	}

}