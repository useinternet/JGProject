#include "pch.h"
#include "MaterialView.h"
#include "UI/UIManager.h"
#include "Application.h"
#include "ExternalImpl/JGImGui.h"
#include "ExternalImpl/JGScriptEditor.h"
#include "Class/Asset/Asset.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/GraphicsHelper.h"

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
		mScriptEditor = CreateUniquePtr<JGScriptEditor>(JVector2(600, 600));
	}
	void MaterialView::OnGUI()
	{
		ImGui::Text("This is Material View");
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, mImageSize.x + ImGui::GetStyle().FramePadding.x * 4);
		LeftTopOnGUI();
		LeftBottomOnGUI();
		ImGui::NextColumn();
		RightTopOnGUI();
		RightBottomOnGUI();
		ImGui::Columns(1);


	}
	void MaterialView::Destroy()
	{
		mMeshAsset = nullptr;
		mMaterialAsset = nullptr;

		JGGraphics::GetInstance().DestroyObject(mScene);
		mScene = nullptr;
		if (mScriptEditor != nullptr)
		{
			mScriptEditor->Destroy();
			mScriptEditor = nullptr;
		}
	}
	void MaterialView::OnEvent(IEvent& e)
	{


	}
	void MaterialView::LeftTopOnGUI()
	{
		u64 texID = 0;

		PushSceneObject();
		Rendering();

		JVector2 padding = JVector2(ImGui::GetStyle().FramePadding.x * 2, ImGui::GetStyle().FramePadding.y * 2);
		JVector2 winSize = mResolution + padding;

		ImGui::BeginChild("LeftTop_OnGUI", ImVec2(winSize.x, winSize.y));
		if (mSceneTexture == nullptr)
		{
			texID = JGImGui::GetInstance().ConvertImGuiTextureID(ITexture::NullTexture()->GetTextureID());
		}
		else
		{
			texID = JGImGui::GetInstance().ConvertImGuiTextureID(mSceneTexture->GetTextureID());
		}

		ImGui::ImageButton((ImTextureID)texID, ImVec2(mImageSize.x, mImageSize.y), ImVec2(0, 0), ImVec2(1, 1), 0);

		if (ImGui::IsItemHovered() == true && ImGui::IsMouseDown(ImGuiMouseButton_Left) == true)
		{
			static JVector2 NullMousePosition = JVector2(-1, -1);
			ImVec2 imMousePos = ImGui::GetMousePos();
			JVector2 delta = JVector2(0, 0);
			if (mMousePosition == NullMousePosition)
			{
				mMousePosition = JVector2(imMousePos.x, imMousePos.y);
				mPrevMousePosition = mMousePosition;
			}
			else
			{
				mMousePosition = JVector2(imMousePos.x, imMousePos.y);
				delta = mMousePosition - mPrevMousePosition;
				mPrevMousePosition = mMousePosition;
			}
			mModelRotation.x += Math::ConvertToRadians(delta.y);
			mModelRotation.y += Math::ConvertToRadians(delta.x);
			JG_LOG_INFO("Image Drag  {0} , {1} ", delta.x, delta.y);
		}
		else
		{
			mMousePosition = JVector2(-1, -1);
			mPrevMousePosition = JVector2(-1, -1);
		}
		ImGui::EndChild();
	}
	void MaterialView::LeftBottomOnGUI()
	{
		ImGui::BeginChild("LeftBottom_OnGUI");
		ImGui::Text("Property");
		ImGui::EndChild();
	}
	void MaterialView::RightTopOnGUI()
	{
		mScriptEditor->OnGUI();
		//ImGui::BeginChild("RightTop_OnGUI", ImVec2(640, 640));

		//static char test[512] = {};
		//ImGui::InputTextMultiline("TextEditor", test, 512, ImVec2(600, 600));
		//ImGui::EndChild();
	}
	void MaterialView::RightBottomOnGUI()
	{
		ImGui::BeginChild("RightBottom_OnGUI");
		ImGui::Text("Compile");
		ImGui::Button("Compile");
		ImGui::EndChild();
	}
	void MaterialView::PushSceneObject()
	{
		if (mSkyBox == nullptr)
		{
			mSkyBox = GraphicsHelper::CreateSkyBox(mEyePos, mFarZ, "Asset/Engine/CubeMap/DefaultSky.jgasset");
		}
		if (mMeshAsset == nullptr)
		{
			String enginePath = Application::GetEnginePath();
			mMeshAsset = AssetDataBase::GetInstance().LoadOriginAsset<IMesh>(PathHelper::CombinePath(enginePath, "Mesh/Sphere.jgasset"));
		}
		if (mScene != nullptr && mScene->IsEnableRendering())
		{
			if (mMeshAsset != nullptr && mMeshAsset->IsValid() == true && mMaterialAsset != nullptr && mMaterialAsset->IsValid() == true)
			{
				SharedPtr<Graphics::StaticRenderObject> sceneObject = CreateSharedPtr<Graphics::StaticRenderObject>();
				JQuaternion q = JQuaternion::RotationRollPitchYawFromVector(JVector3(mModelRotation.x, mModelRotation.y, 0.0f));
				sceneObject->WorldMatrix = JMatrix::Rotation(q) * JMatrix::Scaling(1.5f);
				sceneObject->Mesh = mMeshAsset->Get();
				sceneObject->MaterialList.push_back(mMaterialAsset->Get());
				mScene->PushSceneObject(sceneObject);
			}
			if (mSkyBox != nullptr)
			{
				mScene->PushSceneObject(mSkyBox);
			}
			// 
			SharedPtr<Graphics::DirectionalLight> dl = CreateSharedPtr<Graphics::DirectionalLight>();
			dl->Color = Color::White();
			dl->Direction = JVector3(-0.2f, -0.8f, 1.0f);
			dl->Intensity = 1.0f;
			mScene->PushLight(dl);

		}
	}
	void MaterialView::Rendering()
	{
		if (mScene != nullptr)
		{

			SharedPtr<Graphics::SceneResultInfo> resultInfo = mScene->FetchResult();
			if (resultInfo != nullptr && resultInfo->Texture != nullptr && resultInfo->Texture->IsValid() == true)
			{
				mSceneTexture = resultInfo->Texture;
			}
			mScene->Rendering();
		}
	}
	void MaterialView::UpdateScene()
	{
		const JVector3 targetVec(0, 0, -1);
		const JVector3 upVec(0, 1, 0);


		Graphics::SceneInfo sceneInfo;
		sceneInfo.RenderPath = ERendererPath::RayTracing;
		sceneInfo.Resolution = mResolution;
		sceneInfo.EyePos = mEyePos;
		sceneInfo.ViewMatrix = JMatrix::LookAtLH(mEyePos, targetVec, upVec);
		sceneInfo.ProjMatrix = JMatrix::PerspectiveFovLH(Math::ConvertToRadians(90), mResolution.x / mResolution.y, mNearZ, mFarZ);
		sceneInfo.ViewProjMatrix = sceneInfo.ViewMatrix * sceneInfo.ProjMatrix;
		sceneInfo.NearZ = mNearZ;
		sceneInfo.FarZ = mFarZ;
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
		AssetID originMaterial = AssetDataBase::GetInstance().GetAssetOriginID(path);
		mMaterialAsset = AssetDataBase::GetInstance().LoadReadWriteAsset<IMaterial>(originMaterial);

		if (mScriptEditor != nullptr && mMaterialAsset != nullptr && mMaterialAsset->IsValid())
		{
			SharedPtr<IMaterial> m = mMaterialAsset->Get();
			auto script = m->GetScript();
			if (script != nullptr)
			{
				mScriptEditor->Load(mMaterialAsset->GetAssetName(), script->GetCode());
			}
		}
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