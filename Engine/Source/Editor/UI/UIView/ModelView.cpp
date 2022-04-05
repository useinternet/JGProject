#include "pch.h"
#include "ModelView.h"
#include "UI/UIManager.h"
#include "ExternalImpl/JGImGui.h"
#include "ExternalImpl/ImGuiExternal.h"
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
		UpdateScene();
	}
	void ModelView::OnGUI()
	{
		
		Top_OnGUI();
		Mid_OnGUI();
		Bottom_OnGUI();
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
		if (mScene != nullptr && mScene->IsEnableRendering())
		{
			if (mMeshAsset != nullptr && mMeshAsset->IsValid() == true && mMaterialAssetList.empty() == false)
			{
				SharedPtr<Graphics::StaticRenderObject> sceneObject = CreateSharedPtr<Graphics::StaticRenderObject>();
				JQuaternion q = JQuaternion::RotationRollPitchYawFromVector(JVector3(mModelRotation.x, mModelRotation.y, 0.0f));
				sceneObject->WorldMatrix = JMatrix::Rotation(q);
				sceneObject->Mesh        = mMeshAsset->Get();

				for (auto& m : mMaterialAssetList)
				{
					if (m == nullptr || m->IsValid() == false) continue;
					sceneObject->MaterialList.push_back(m->Get());
				}
				mScene->PushSceneObject(sceneObject);
			}
		}
	}
	void ModelView::Bottom_OnGUI()
	{

	}
	void ModelView::MidLeft_OnGUI()
	{
		u64 texID = 0;
	
		PushSceneObject();
		Rendering();
	

		ImGui::BeginChild("MideLeft_OnGUI");
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
			JVector2 delta    = JVector2(0, 0);
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
	void ModelView::MidRight_OnGUI()
	{
		ImGui::BeginChild("MidRight_OnGUI");
		if (ImGui::CollapsingHeader("Mesh Info", ImGuiTreeNodeFlags_DefaultOpen) == true && mMeshAsset->IsValid())
		{
			SharedPtr<IMesh> mesh = mMeshAsset->Get();
			const MeshInfo& meshInfo = mesh->GetMeshInfo();

			
			ImGui::Text("Total SubMesh Count : %d", meshInfo.TotalSubMeshCount);
			ImGui::Text("Total Vertex Count : %d", meshInfo.TotalVertexCount);
			ImGui::Text("Total Index  Count : %d", meshInfo.TotalIndexCount);

			if (ImGui::TreeNodeEx("InputLayout") == true)
			{
				i32 i = 0;
				meshInfo.InputLayout->ForEach([&](const InputElement& ele)
				{
					ImGui::Text("Index : %d", i);
					ImGui::Text("DataType : %s", ShaderDataTypeToString(ele.Type).c_str());
					ImGui::Text("Sementic Name %d : %s", ele.SementicSlot, ele.SementicName);
					++i;
				});

				ImGui::TreePop();
			}

			for (u32 i = 0; i < meshInfo.TotalSubMeshCount; ++i)
			{
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.235f, 0.31f, 1.0f));
				bool isOpen = ImGui::TreeNodeEx(meshInfo.SubMeshNames[i].c_str(), ImGuiTreeNodeFlags_DefaultOpen);
				ImGui::PopStyleColor();
				if (isOpen)
				{
					ImGui::Text("Vertex Count : %d", meshInfo.SubMeshVertexCounts[i]);
					ImGui::Text("Index Count  : %d", meshInfo.SubMeshIndexCounts[i]);
					ImGui::TreePop();
				}
			}
		}

		if (ImGui::CollapsingHeader("Materials", ImGuiTreeNodeFlags_DefaultOpen) == true)
		{
			String inputText = "";
			if (mMaterialAssetList.empty() == false)
			{
				inputText = mMaterialAssetList[0]->GetAssetName();
			}

			ImGui::AssetField_OnGUI("Material 0", inputText, EAssetFormat::Material, [&](const std::string path)
			{
				SetMaterial(0, path);
			});
		}
		ImGui::EndChild();




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
			mScene->Rendering();
		}
	}
	void ModelView::Mid_OnGUI()
	{
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, mImageSize.x + ImGui::GetStyle().FramePadding.x * 4);
		MidLeft_OnGUI();
		ImGui::NextColumn();
		MidRight_OnGUI();
		ImGui::Columns(1);
	}
	void ModelView::Top_OnGUI()
	{

	}

	void ModelView::UpdateScene()
	{
		const JVector3 eyePos(0, 0, -300);
		const JVector3 targetVec(0, 0, -1);
		const JVector3 upVec(0, 1, 0);
		const f32 NearZ = 1.0f;
		const f32 FarZ = 1000.0f;


		Graphics::SceneInfo sceneInfo;
		sceneInfo.RenderPath = ERendererPath::RayTracing;
		sceneInfo.Resolution = mResolution;
		sceneInfo.EyePos     = eyePos;
		sceneInfo.ViewMatrix = JMatrix::LookAtLH(eyePos, targetVec, upVec);
		sceneInfo.ProjMatrix = JMatrix::PerspectiveFovLH(Math::ConvertToRadians(90), mResolution.x / mResolution.y, NearZ, FarZ);
		sceneInfo.ViewProjMatrix = sceneInfo.ViewMatrix * sceneInfo.ProjMatrix;
		sceneInfo.NearZ = NearZ;
		sceneInfo.FarZ  = FarZ;
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

			SetTitleName("ModelView_" + asset->GetAssetPath());
		}
	}
	void ModelView::SetMaterial(i32 index, const String& path)
	{
		if (index > 0)
		{
			return;
		}
		if (mMaterialAssetList.empty())
		{
			mMaterialAssetList.resize(1);
		}

		mMaterialAssetList[0] = AssetDataBase::GetInstance().LoadOriginAsset<IMaterial>(path);
	}
}