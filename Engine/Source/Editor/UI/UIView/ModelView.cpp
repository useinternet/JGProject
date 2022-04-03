#include "pch.h"
#include "ModelView.h"
#include "UI/UIManager.h"
#include "ExternalImpl/JGImGui.h"
#include "Graphics/JGGraphics.h"
namespace JG
{
	ModelView::ModelView()
	{
		DisableUniqueView();
	}
	void ModelView::Load()
	{
		
	}
	void ModelView::Initialize()
	{
		Graphics::SceneInfo sceneInfo;
		sceneInfo.RenderPath = ERendererPath::RayTracing;
		sceneInfo.Resolution = mResolution;
		sceneInfo.EyePos = JVector3(0, 0, -300);
		sceneInfo.ViewMatrix = JMatrix::LookAtLH(sceneInfo.EyePos, JVector3(0, 0, -1), JVector3(0, 1, 0));
		sceneInfo.ProjMatrix = JMatrix::PerspectiveFovLH(Math::ConvertToRadians(90), mResolution.x / mResolution.y, 1, 100000.0f);
		sceneInfo.ViewProjMatrix = sceneInfo.ViewMatrix * sceneInfo.ProjMatrix;
		sceneInfo.NearZ = 1.0f;
		sceneInfo.FarZ  = 1000.0f;
		sceneInfo.ClearColor = Color::Green();

		mScene = JGGraphics::GetInstance().CreateScene("ModelView_Scene", sceneInfo);
	}
	void ModelView::OnGUI()
	{
		ImGui::Text("This is Model View");
		Scene_OnGUI();

	}

	void ModelView::PreOnGUI()
	{


	}
	void ModelView::LateOnGUI()
	{

	}
	void ModelView::Destroy()
	{
		mMeshAsset = nullptr;
		mMaterialAssetList.clear();

		JGGraphics::GetInstance().DestroyObject(mScene);
		mScene = nullptr;
	}

	void ModelView::OnEvent(IEvent& e)
	{
	}
	void ModelView::PushSceneObject()
	{
		if (mScene != nullptr)
		{
			if (mMeshAsset != nullptr && mMeshAsset->IsValid() == true && mMaterialAssetList.empty() == false)
			{
				SharedPtr<Graphics::StaticRenderObject> sceneObject = CreateSharedPtr<Graphics::StaticRenderObject>();
				sceneObject->WorldMatrix = JMatrix::Identity();
				sceneObject->Mesh = mMeshAsset->Get();

				for (auto& m : mMaterialAssetList)
				{
					if (m == nullptr || m->IsValid() == false) continue;
					sceneObject->MaterialList.push_back(m->Get());
				}
				mScene->PushSceneObject(sceneObject);
			}
		}
	}
	void ModelView::Right_OnGUI()
	{
	}
	void ModelView::Bottom_OnGUI()
	{
	}
	void ModelView::Rendering()
	{
		if (mScene != nullptr)
		{
			SharedPtr<Graphics::SceneResultInfo> resultInfo = mScene->FetchResult();
			if (resultInfo != nullptr && resultInfo->Texture != nullptr && resultInfo->Texture->IsValid() == true)
			{
				mSceneTexture = resultInfo->Texture;
			}
			else
			{
				mSceneTexture = nullptr;
			}


			mScene->Rendering();
		}
	}
	void ModelView::Scene_OnGUI()
	{
		u64 texID = 0;
		PushSceneObject();
		Rendering();
		if (mSceneTexture == nullptr)
		{
			texID = JGImGui::GetInstance().ConvertImGuiTextureID(ITexture::NullTexture()->GetTextureID());
		}
		else
		{
			texID = JGImGui::GetInstance().ConvertImGuiTextureID(mSceneTexture->GetTextureID());
		}
		ImGui::ImageButton((ImTextureID)texID, ImVec2(mResolution.x * 1.5f, mResolution.y * 1.5f), ImVec2(0,0), ImVec2(1,1), 0);
	}
	void ModelView::Top_OnGUI()
	{
	}
	void ModelView::SetModel(const String& path)
	{
		SharedPtr<Asset<IMesh>> asset =  AssetDataBase::GetInstance().LoadOriginAsset<IMesh>(path);
		if (asset != nullptr)
		{
			mMeshAsset = asset;


			String defaultMaterialPath		= PathHelper::CombinePath(Application::GetEnginePath(), "Material/M_Default.jgasset");
			SharedPtr<Asset<IMaterial>> materialAsset = AssetDataBase::GetInstance().LoadOriginAsset<IMaterial>(defaultMaterialPath);
			if (materialAsset != nullptr)
			{
				mMaterialAssetList.push_back(materialAsset);
			}
		}
	}

}