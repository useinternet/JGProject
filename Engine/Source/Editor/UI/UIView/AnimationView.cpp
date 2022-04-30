#include "pch.h"
#include "AnimationView.h"
#include "Application.h"
#include "ExternalImpl/JGImGui.h"
#include "ExternalImpl/TextEditor.h"
#include "Class/Asset/Asset.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/GraphicsHelper.h"
#include "UI/UIManager.h"
#include "UI/EditorUIScene.h"


namespace JG
{
	AnimationView::AnimationView()
	{
	}

	void AnimationView::Load()
	{


	}

	void AnimationView::Initialize()
	{
	}

	void AnimationView::OnGUI()
	{
	}

	void AnimationView::PreOnGUI()
	{
	}

	void AnimationView::LateOnGUI()
	{
	}

	void AnimationView::Destroy()
	{
	}

	void AnimationView::OnEvent(IEvent& e)
	{
	}

	void AnimationView::UpdateScene()
	{
		const JVector3 targetVec(0, 0, -1);
		const JVector3 upVec(0, 1, 0);
		const JVector3 eyePos(0, 0, -200.0f);
		const f32 farZ  = 1000000.0f;
		const f32 nearZ = 1.0f;

		//if (mSkyBox == nullptr)
		//{
		//	mSkyBox = GraphicsHelper::CreateSkyBox(eyePos, farZ, "Asset/Engine/CubeMap/DefaultSky.jgasset");
		//}
		//if (mModel == nullptr)
		//{
		//	mModel = CreateSharedPtr<Graphics::StaticRenderObject>();
		//	if (mMaterialAsset != nullptr && mMaterialAsset->IsValid())
		//	{
		//		mModel->MaterialList.resize(1);
		//		mModel->MaterialList[0] = mMaterialAsset->Get();
		//	}
		//}
		//if (mModel->Mesh == nullptr)
		//{
		//	String enginePath = Application::GetEnginePath();
		//	auto   meshAsset = AssetDataBase::GetInstance().LoadOriginAsset<IMesh>(PathHelper::CombinePath(enginePath, "Mesh/Sphere.jgasset"));
		//	if (meshAsset != nullptr && meshAsset->IsValid())
		//	{
		//		mModel->Mesh = meshAsset->Get();
		//	}
		//}
		//if (mEditorUIScene == nullptr)
		//{
		//	EditorUISceneConfig config;
		//	config.Resolution = mResolution;
		//	config.ImageSize = mImageSize;
		//	config.OffsetScale = JVector3(1.5f, 1.5f, 1.5f);
		//	config.Model = mModel;
		//	config.SkyBox = mSkyBox;
		//	config.FarZ = farZ;
		//	config.NearZ = nearZ;
		//	config.EyePos = eyePos;

		//	mEditorUIScene = CreateUniquePtr<EditorUIScene>(config);
		//}
	}

}