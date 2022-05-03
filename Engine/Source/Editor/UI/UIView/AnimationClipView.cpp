#include "pch.h"
#include "AnimationClipView.h"
#include "Application.h"
#include "ExternalImpl/JGImGui.h"
#include "ExternalImpl/TextEditor.h"
#include "Class/Asset/Asset.h"
#include "Class/Data/Skeletone.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/GraphicsHelper.h"
#include "UI/UIManager.h"
#include "UI/EditorUIScene.h"

#include "Animation/JGAnimation.h"
#include "Animation/AnimationController.h"
#include "Animation/AnimationSequence.h"
#include "Animation/AnimationClip.h"
namespace JG
{
	AnimationClipView::AnimationClipView()
	{
		DisableUniqueView();
	}

	void AnimationClipView::Load()
	{

	}

	void AnimationClipView::Initialize()
	{
		SetMesh(mModelAssetPath.GetValue());
		SetSkeletal(mSkeletoneAssetPath.GetValue());
		SetMaterial(StringHelper::Split(mMaterialAssetPath.GetValue(), ','));
	}

	void AnimationClipView::OnGUI()
	{
		
		JVector2 winSize;
		f32 LeftWidth  = mSceneResolution.x + ImGui::GetStyle().FramePadding.x * 4;
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

	void AnimationClipView::PreOnGUI()
	{
	}

	void AnimationClipView::LateOnGUI()
	{
	}

	void AnimationClipView::Destroy()
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
		mAnimClipAsset = nullptr;
		mSkyBox = nullptr;
		mModel  = nullptr;

		if (mAnimController != nullptr)
		{
			mAnimController->RemoveAnimationClip(CLIP_NAME);
			JGAnimation::GetInstance().UnRegisterAnimatioinController(mAnimController);
			mAnimController = nullptr;
		}
		mEditorUIScene = nullptr;
	}

	void AnimationClipView::OnEvent(IEvent& e)
	{
	}

	void AnimationClipView::LeftOnGUI()
	{
		ImGui::BeginChild("AnimationClipView_SceneView");
		if (mEditorUIScene != nullptr)
		{
			mEditorUIScene->OnGUI();
		}
		ImGui::Separator();
		ImGui::Button("TestButton");
		ImGui::EndChild();
	}
	void AnimationClipView::RightOnGUI()
	{
		ImGui::BeginChild("AnimationClipView_Inspector");

		f32 label_Space = ImGui::CalcTextSize("          ").x;
		
		{
			ImGui::AssetField_OnGUI("Mesh", (mMeshAsset != nullptr && mMeshAsset->IsValid()) ? mMeshAsset->GetAssetName() : "None",
				EAssetFormat::Mesh, [&](const String& assetPath)
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
			},label_Space);
		}

		{
			ImGui::AssetField_OnGUI("Skeletal", (mSkeletoneAsset != nullptr && mSkeletoneAsset->IsValid()) ? mSkeletoneAsset->GetAssetName() : "None",
				EAssetFormat::Skeletal, [&](const String& assetPath)
			{
				SetSkeletal(assetPath);
			}, label_Space);
		}
		ImGui::EndChild();
	}

	void AnimationClipView::SetMesh(const String& meshAssetPath)
	{
		mMeshAsset = AssetDataBase::GetInstance().LoadOriginAsset<IMesh>(meshAssetPath);
	}
	void AnimationClipView::SetSkeletal(const String& skeletalAssetPath)
	{
		mSkeletoneAsset = AssetDataBase::GetInstance().LoadOriginAsset<Skeletone>(skeletalAssetPath);
	}
	void AnimationClipView::SetAnimationClip(const String& animClipPath)
	{
		mAnimClipAsset = AssetDataBase::GetInstance().LoadOriginAsset<AnimationClip>(animClipPath);
		if (mAnimClipAsset)
		{
			mModelAssetPath     = StorableString("AnimationClipView/ModelAssetPath/" + mAnimClipAsset->GetAssetPath(), "");
			mSkeletoneAssetPath = StorableString("AnimationClipView/SkeletoneAssetPath/" + mAnimClipAsset->GetAssetPath(), "");
			mMaterialAssetPath  = StorableString("AnimationClipView/MaterialAssetPath/" + mAnimClipAsset->GetAssetPath(), "");
			SetTitleName(mAnimClipAsset->GetAssetPath());
		}
		
	}
	void AnimationClipView::SetMaterial(const List<String>& materialAssetPath)
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

	void AnimationClipView::UpdateScene()
	{
		// Scene 持失
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
		}




		if (mMeshAsset != nullptr && mMeshAsset->IsValid())
		{
			mModel->Mesh = mMeshAsset->Get();
			JBBox boundingBox = mModel->Mesh->GetBoundingBox();

			JVector3 Center = boundingBox.Center();
			mEditorUIScene->SetLocation(Center * -1);

			JVector3 targetVec = EyePos * -1;
			mEditorUIScene->SetTargetVector(targetVec);

			if (mAnimController != nullptr)
			{
				mAnimController->BindMesh(mMeshAsset->Get());
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


		// Animation 持失
		if (mAnimClipAsset != nullptr && mAnimClipAsset->IsValid())
		{
			if (mAnimController == nullptr)
			{
				mAnimController = CreateUniquePtr<AnimationController>();
				JGAnimation::GetInstance().RegisterAnimationController(mAnimController);

				mAnimController->AddAnimationClip(CLIP_NAME, mAnimClipAsset->Get(), EAnimationClipFlags::None);
				mAnimController->GetRootAnimationSequence()->
					 Begin("Start")
					.MakeAnimationClipNode(CLIP_NAME, nullptr)
					.ConnectNode("Start", CLIP_NAME, nullptr)
					.End();

				
			}


			if (mSkeletoneAsset != nullptr && mSkeletoneAsset->IsValid())
			{
				mAnimController->BindSkeletone(mSkeletoneAsset->Get());
			}

			SharedPtr<AnimationTransform> transform = mAnimController->GetFinalTransform();
		}
	}

}