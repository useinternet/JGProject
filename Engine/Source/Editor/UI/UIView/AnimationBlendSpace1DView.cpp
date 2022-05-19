#include "pch.h"
#include "AnimationBlendSpace1DView.h"
#include "Application.h"
#include "ExternalImpl/JGImGui.h"
#include "ExternalImpl/TextEditor.h"
#include "Class/Asset/Asset.h"
#include "Class/Data/Skeletone.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/GraphicsHelper.h"
#include "UI/UIManager.h"
#include "UI/EditorUIScene.h"

#include "Class/Asset/Asset.h"
#include "Class/Asset/AssetHelper.h"
#include "Animation/JGAnimation.h"
#include "Animation/AnimationController.h"
#include "Animation/AnimationStateMachine.h"
#include "Animation/AnimationClip.h"
#include "Animation/AnimationBlendSpace1D.h"
#include "Animation/AnimationParameters.h"


namespace JG
{
	AnimationBlendSpace1DView::AnimationBlendSpace1DView()
	{
		DisableUniqueView();

	}

	void AnimationBlendSpace1DView::Initialize()
	{
		SetMesh(mModelAssetPath.GetValue());
		SetSkeletal(mSkeletoneAssetPath.GetValue());
		SetMaterial(StringHelper::Split(mMaterialAssetPath.GetValue(), ','));


		if (mAnimBlendAsset != nullptr && mAnimBlendAsset->IsValid())
		{
			AnimationBlendSpace1DStock stock;
			AssetHelper::ReadAsset(EAssetFormat::AnimationBlendSpace1D, mAnimBlendAsset->GetAssetPath(), [&](SharedPtr<JsonData> jsonData)
			{
				stock.LoadJson(jsonData);
			});
			mBuildData.XParamName = stock.XParamName;
			for (const AnimationBlendSpace1DStock::AnimClipData& clipData : stock.AnimClipDatas)
			{
				AnimClipData data;
				data.Name = clipData.Name;
				data.Asset = AssetDataBase::GetInstance().LoadOriginAssetImmediate<AnimationClip>(clipData.AssetPath);
				data.Value = clipData.Value;
				mBuildData.AnimClipDatas.push_back(data);
			}
			mBuildData.MinMaxValue = stock.MinMaxValue;
		}
	}

	void AnimationBlendSpace1DView::OnGUI()
	{

		JVector2 winSize;
		f32 LeftWidth = mSceneResolution.x + ImGui::GetStyle().FramePadding.x * 4;
		f32 RightWidth = mInspectorWidth + ImGui::GetStyle().FramePadding.x * 4;
		winSize.x = LeftWidth + RightWidth;
		winSize.y = mSceneResolution.y + 65.0f + ImGui::GetStyle().FramePadding.y * 2;
		ImGui::SetWindowSize(ImVec2(winSize.x, winSize.y));
		// Image
		UpdateScene();
		ImGui::Columns(2);
		ImGui::SetColumnWidth(-1, LeftWidth);
		LeftOnGUI();
		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, RightWidth);
		RightOnGUI();
		ImGui::Columns(1);
	}

	void AnimationBlendSpace1DView::Destroy()
	{
		if (mMeshAsset != nullptr && mMeshAsset->IsValid())
		{
			mModelAssetPath.SetValue(mMeshAsset->GetAssetPath());
		}
		if (mSkeletoneAsset != nullptr && mSkeletoneAsset->IsValid())
		{
			mSkeletoneAssetPath.SetValue(mSkeletoneAsset->GetAssetPath());
		}

		String materialAssetPathStr;
		for (SharedPtr<Asset<IMaterial>> material : mMaterialAssetList)
		{
			if (material != nullptr && material->IsValid())
			{
				materialAssetPathStr += material->GetAssetPath() + ",";
			}
		}
		mMaterialAssetPath.SetValue(materialAssetPathStr);


		mMaterialAssetList.clear();
		mAnimBlendAsset = nullptr;
		mSkyBox = nullptr;
		mModel = nullptr;
		mBuildData = AnimationBlendSpace1DBuildData();
		if (mAnimController != nullptr)
		{
			JGAnimation::GetInstance().UnRegisterAnimatioinController(mAnimController);
			mAnimController = nullptr;
		}
		mEditorUIScene = nullptr;
	}

	void AnimationBlendSpace1DView::OnEvent(IEvent& e)
	{
	}

	void AnimationBlendSpace1DView::Build()
	{
		if (mAnimBlendAsset == nullptr) return;
		AnimationBlendSpace1DStock stock;

		stock.Name = mAnimBlendAsset->Get()->GetName();
		stock.XParamName = mBuildData.XParamName;
		for (const AnimClipData& clipData : mBuildData.AnimClipDatas)
		{
			if (clipData.Asset == nullptr || clipData.Asset->IsValid() == false)
			{
				continue;
			}

			AnimationBlendSpace1DStock::AnimClipData data;
			data.AssetPath = clipData.Asset->GetAssetPath();
			data.Name      = clipData.Name;
			data.Value     = clipData.Value;
			stock.AnimClipDatas.push_back(data);
		}
		stock.MinMaxValue = mBuildData.MinMaxValue;


		if (AssetHelper::WriteAsset(EAssetFormat::AnimationBlendSpace1D, mAnimBlendAsset->GetAssetPath(),
			[&](SharedPtr<JsonData> assetJson)
		{
			stock.MakeJson(assetJson);
		}) == false)
		{
			JG_LOG_ERROR("Fail Write Asset");
		}

		RefreshAsset(mAnimBlendAsset->GetAssetPath());
	}

	void AnimationBlendSpace1DView::RefreshAsset(const String& assetPath)
	{
		if (mAnimBlendAsset == nullptr)
		{
			mAnimBlendAsset = AssetDataBase::GetInstance().LoadOriginAssetImmediate<AnimationBlendSpace1D>(assetPath);
		}
		else
		{
			AssetDataBase::GetInstance().RefreshAsset(mAnimBlendAsset->GetAssetID(), true);
		}


		if (mAnimController != nullptr)
		{
			JGAnimation::GetInstance().UnRegisterAnimatioinController(mAnimController);
		}
		if (mAnimBlendAsset != nullptr && mAnimBlendAsset->IsValid())
		{
			mAnimController = CreateUniquePtr<AnimationController>();
			mAnimController->GetAnimationParameters()->SetFloat(mAnimBlendAsset->Get()->GetXParamName(), 0.0f);
			mAnimController->AddAnimationBlendSpace1D(BLEND_SPACE_NAME, mAnimBlendAsset->Get(), EAnimationBlendSpace1DFlag::Repeat);
			mAnimController->GetAnimationStateMachine()->
				Begin("Start")
				.MakeAnimationBlendSpace1DNode(BLEND_SPACE_NAME, nullptr)
				.ConnectNode("Start", BLEND_SPACE_NAME, nullptr)
				.End();

			JGAnimation::GetInstance().RegisterAnimationController(mAnimController);
		}


	}

	void AnimationBlendSpace1DView::LeftOnGUI()
	{
		ImGui::BeginChild("AnimationBlendSpace1DView_SceneView");
		if (mEditorUIScene != nullptr)
		{
			mEditorUIScene->OnGUI();
		}
		if (ImGui::Button("Build") == true)
		{
			Build();
		}
		ImGui::EndChild();
	}
	void AnimationBlendSpace1DView::RightOnGUI()
	{
		ImGui::BeginChild("AnimationClipView_Inspector");

		f32 label_Space = ImGui::CalcTextSize("          ").x;

		{
			ImGui::AssetField_OnGUI("Mesh", (mMeshAsset != nullptr && mMeshAsset->IsValid()) ? mMeshAsset->GetAssetName() : "None",
				EAssetFormat::SkeletalMesh, [&](const String& assetPath)
			{
				SetMesh(assetPath);
			}, label_Space);
		}

		{
			List<String> inputText;
			for (SharedPtr<Asset<IMaterial>> material : mMaterialAssetList)
			{
				if (material == nullptr)
				{
					inputText.push_back("None");
				}
				else
				{
					inputText.push_back(material->GetAssetName());
				}
			}
			ImGui::AssetField_List_OnGUI("Material", inputText, EAssetFormat::Material,
				[&](int i, const String& assetPath)
			{
				SharedPtr<Asset<IMaterial>> originMaterial = AssetDataBase::GetInstance().LoadOriginAsset<IMaterial>(assetPath);
				if (originMaterial != nullptr)
				{
					mMaterialAssetList[i] = AssetDataBase::GetInstance().LoadReadWriteAsset<IMaterial>(originMaterial->GetAssetID());
				}
			},
				[&]()
			{
				mMaterialAssetList.push_back(nullptr);
			},
				[&]()
			{
				mMaterialAssetList.pop_back();
			}, label_Space);
		}

		{
			ImGui::AssetField_OnGUI("Skeletal", (mSkeletoneAsset != nullptr && mSkeletoneAsset->IsValid()) ? mSkeletoneAsset->GetAssetName() : "None",
				EAssetFormat::Skeletal, [&](const String& assetPath)
			{
				SetSkeletal(assetPath);
			}, label_Space);
		}

		ImGui::Separator();
		ImGui::Text("ParamName"); ImGui::SameLine();
		String inputText;
		ImGui::SetNextItemWidth(150.0F);
		ImGui::InputText("##X_ParamName_InputText", mBuildData.XParamName, inputText);
		if (ImGui::IsItemDeactivated())
		{
			mBuildData.XParamName = inputText;
		} ImGui::SameLine();


		ImGui::Text(" Min"); ImGui::SameLine();
		f32 minValue = mBuildData.MinMaxValue.x;
		ImGui::SetNextItemWidth(60.0f);
		ImGui::InputFloat("##Float_MinValue", &minValue); 
		if (ImGui::IsItemDeactivated())
		{
			mBuildData.MinMaxValue.x = minValue;
		}ImGui::SameLine();

		ImGui::Text("Max"); ImGui::SameLine();
		f32 maxValue = mBuildData.MinMaxValue.y;
		ImGui::SetNextItemWidth(60.0f);
		ImGui::InputFloat("##Float_MaxValue", &maxValue);
		if (ImGui::IsItemDeactivated())
		{
			mBuildData.MinMaxValue.y = maxValue;
		}
		
		



		if (mAnimController != nullptr)
		{
			f32 value = 0.0f;
			SharedPtr<AnimationParameters> animParam = mAnimController->GetAnimationParameters();
			animParam->GetFloat(mBuildData.XParamName, &value);

			if (ImGui::SliderFloat("##SliderFloat", &value, mBuildData.MinMaxValue.x, mBuildData.MinMaxValue.y, "%3.f", 1.0f) == true)
			{
				animParam->SetFloat(mBuildData.XParamName, value);
			}
		}

		ImGui::Separator();


		static f32 nameRowWidth = 120.0f;
		static f32 assetFieldRowWidth = 200.0f;
		static f32 valueRowWidth = 80.0f;
		static f32 buttonRowWidth = 40.0f;
		// Animation Clip 추가
		ImGui::BeginTable("AnimClipTable", 4, ImGuiTableFlags_BordersH);
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, nameRowWidth);
		ImGui::TableSetupColumn("AnimClip", ImGuiTableColumnFlags_WidthFixed, assetFieldRowWidth);
		ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, valueRowWidth);
		ImGui::TableSetupColumn("##Button", ImGuiTableColumnFlags_WidthFixed, buttonRowWidth);
		ImGui::TableHeadersRow();
		ImVec2 padding = ImGui::GetStyle().FramePadding;

		i32 index = 0;
		i32 removeIndex = -1;
		for (AnimClipData& clipData : mBuildData.AnimClipDatas)
		{
			ImGui::PushID(index);
			
			String inputText;
			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(nameRowWidth);
			ImGui::InputText("##InputText", clipData.Name, inputText);
			if (ImGui::IsItemDeactivated())
			{
				clipData.Name = inputText;
			}

			ImGui::TableNextColumn();
			ImGui::AssetField_OnGUI("##AnimClip", (clipData.Asset != nullptr && clipData.Asset->IsValid()) ? clipData.Asset->GetAssetName() : "None",
				EAssetFormat::AnimationClip, [&](const String& assetPath)
			{
				clipData.Asset = AssetDataBase::GetInstance().LoadOriginAsset<AnimationClip>(assetPath);
			});

			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(valueRowWidth);
			// Value
			ImGui::InputFloat("##InputFloat", &clipData.Value);


			ImGui::TableNextColumn();
			//삭제
			if (ImGui::Button("-", ImVec2(buttonRowWidth, 0.0f)) == true)
			{
				removeIndex = index;
			}

			ImGui::PopID();
			index++;
		}

		ImGui::EndTable();
		if (ImGui::Button("+", ImVec2(ImGui::GetWindowContentRegionWidth() - (padding.x * 4), 0.0f)) == true)
		{
			mBuildData.AnimClipDatas.push_back(AnimClipData());
		}

		if (removeIndex >= 0)
		{
			mBuildData.AnimClipDatas.erase(mBuildData.AnimClipDatas.begin() + removeIndex);
		}

		ImGui::EndChild();
	}

	void AnimationBlendSpace1DView::SetMesh(const String& meshAssetPath)
	{
		mMeshAsset = AssetDataBase::GetInstance().LoadOriginAsset<IMesh>(meshAssetPath);
	}
	void AnimationBlendSpace1DView::SetSkeletal(const String& skeletalAssetPath)
	{
		mSkeletoneAsset = AssetDataBase::GetInstance().LoadOriginAsset<Skeletone>(skeletalAssetPath);
	}
	void AnimationBlendSpace1DView::SetAnimationBlendSpace1D(const String& blendSpace1DPath)
	{
		RefreshAsset(blendSpace1DPath);
		if (mAnimBlendAsset)
		{
			mModelAssetPath     = StorableString("AnimationBlendSpace1DView/ModelAssetPath/" + mAnimBlendAsset->GetAssetPath(), "");
			mSkeletoneAssetPath = StorableString("AnimationBlendSpace1DView/SkeletoneAssetPath/" + mAnimBlendAsset->GetAssetPath(), "");
			mMaterialAssetPath  = StorableString("AnimationBlendSpace1DView/MaterialAssetPath/" + mAnimBlendAsset->GetAssetPath(), "");
			SetTitleName(mAnimBlendAsset->GetAssetPath());
		}

	}
	void AnimationBlendSpace1DView::SetMaterial(const List<String>& materialAssetPath)
	{
		mMaterialAssetList.clear();
		for (const String& path : materialAssetPath)
		{
			SharedPtr<Asset<IMaterial>> originMaterial = AssetDataBase::GetInstance().LoadOriginAsset<IMaterial>(path);
			if (originMaterial == nullptr)
			{
				continue;
			}

			mMaterialAssetList.push_back(AssetDataBase::GetInstance().LoadReadWriteAsset<IMaterial>(originMaterial->GetAssetID()));

		}
	}

	void AnimationBlendSpace1DView::UpdateScene()
	{
		// Scene 생성
		static const JVector3 EyePos = JVector3(0, 100.0f, -200.0f);
		if (mEditorUIScene == nullptr)
		{
			EditorUISceneConfig config;
			config.EyePos = EyePos;
			config.Resolution = mSceneResolution;
			config.ImageSize = config.Resolution;
			config.OffsetScale = JVector3(1, 1, 1);
			config.Flags = EEditorUISceneFlags::Fix_RotatePitch;
			config.SkyBox = GraphicsHelper::CreateSkyBox(config.EyePos, config.FarZ, "Asset/Engine/CubeMap/DefaultSky.jgasset");
			mEditorUIScene = CreateUniquePtr<EditorUIScene>(config);
		}

		if (mModel == nullptr)
		{
			mModel = CreateSharedPtr<Graphics::StaticRenderObject>();
			mModel->Flags = Graphics::ESceneObjectFlags::Always_Update_Bottom_Level_AS;
		}

		if (mMeshAsset != nullptr && mMeshAsset->IsValid())
		{
			JBBox boundingBox = mMeshAsset->Get()->GetBoundingBox();

			JVector3 Center = boundingBox.Center();
			mEditorUIScene->SetLocation(Center * -1);

			JVector3 targetVec = JVector3::Normalize(EyePos * -1);
			targetVec.y = Math::Clamp(targetVec.y - 0.15f, 0.0f, targetVec.y);
			mEditorUIScene->SetTargetVector(targetVec);

			if (mAnimController != nullptr)
			{
				mAnimController->BindMesh(mMeshAsset->Get());
				mModel->Mesh = mAnimController->GetBindedMesh();
			}
			else mModel->Mesh = mMeshAsset->Get();
		}
		if (mSkeletoneAsset != nullptr && mSkeletoneAsset->IsValid())
		{
			if (mAnimController != nullptr)
			{
				mAnimController->BindSkeletone(mSkeletoneAsset->Get());
			}
		}
		mModel->MaterialList.clear();
		for (SharedPtr<Asset<IMaterial>> material : mMaterialAssetList)
		{
			if (material == nullptr || material->IsValid() == false)
			{
				continue;
			}
			mModel->MaterialList.push_back(material->Get());
		}



		mEditorUIScene->SetModel(mModel);
	}

}