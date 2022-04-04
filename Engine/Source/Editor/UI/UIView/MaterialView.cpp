#include "pch.h"
#include "MaterialView.h"
#include "UI/UIManager.h"
#include "Class/Asset/Asset.h"
#include "Graphics/JGGraphics.h"



namespace JG
{
	MaterialView::MaterialView()
	{
		DisableUniqueView();


	}
	void MaterialView::Load()
	{

	}
	void MaterialView::Initialize()
	{
		UpdateScene();
	}
	void MaterialView::OnGUI()
	{
		ImGui::Text("This is Material View");



	}
	void MaterialView::Destroy()
	{
		mMeshAsset = nullptr;
		mMaterialAsset = nullptr;

		JGGraphics::GetInstance().DestroyObject(mScene);
		mScene = nullptr;
	}
	void MaterialView::OnEvent(IEvent& e)
	{


	}
	void MaterialView::UpdateScene()
	{
		const JVector3 eyePos(0, 0, -300);
		const JVector3 targetVec(0, 0, -1);
		const JVector3 upVec(0, 1, 0);
		const f32 NearZ = 1.0f;
		const f32 FarZ = 1000.0f;


		Graphics::SceneInfo sceneInfo;
		sceneInfo.RenderPath = ERendererPath::RayTracing;
		sceneInfo.Resolution = mResolution;
		sceneInfo.EyePos = eyePos;
		sceneInfo.ViewMatrix = JMatrix::LookAtLH(eyePos, targetVec, upVec);
		sceneInfo.ProjMatrix = JMatrix::PerspectiveFovLH(Math::ConvertToRadians(90), mResolution.x / mResolution.y, NearZ, FarZ);
		sceneInfo.ViewProjMatrix = sceneInfo.ViewMatrix * sceneInfo.ProjMatrix;
		sceneInfo.NearZ = NearZ;
		sceneInfo.FarZ = FarZ;
		sceneInfo.ClearColor = Color();

		if (mScene == nullptr)
		{

			mScene = JGGraphics::GetInstance().CreateScene("ModelView_Scene", sceneInfo);
		}
		else
		{
			mScene->SetSceneInfo(sceneInfo);
		}

	}

	void MaterialView::SetMaterial(const String& path)
	{
		mMaterialAsset = AssetDataBase::GetInstance().LoadOriginAsset<IMaterial>(path);
	}
	void MaterialView::SetMesh(const String& path)
	{
		SharedPtr<Asset<IMesh>> asset = AssetDataBase::GetInstance().LoadOriginAsset<IMesh>(path);
		if (asset != nullptr && asset->IsValid())
		{
			mMeshAsset = asset;
		}
	}
}