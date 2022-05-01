#include "pch.h"
#include "EditorUIScene.h"
#include "Application.h"
#include "ExternalImpl/JGImGui.h"
#include "Class/Asset/Asset.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/GraphicsHelper.h"
#include "Graphics/Resource.h"
#include "UI/UIManager.h"
#include "imgui/imgui.h"
namespace JG
{
	EditorUIScene::EditorUIScene(const EditorUISceneConfig& config)
	{
		mConfig = config;
		Init();
	}

	EditorUIScene::~EditorUIScene()
	{
		JGGraphics::GetInstance().DestroyObject(mScene);
	}

	void EditorUIScene::OnGUI()
	{
		if (mScene == nullptr)
		{
			return;
		}
		u64 texID = 0;
		if (mSceneTexture == nullptr)
		{
			texID = JGImGui::GetInstance().ConvertImGuiTextureID(ITexture::NullTexture()->GetTextureID());
		}
		else
		{
			texID = JGImGui::GetInstance().ConvertImGuiTextureID(mSceneTexture->GetTextureID());
		}

		ImGui::ImageButton((ImTextureID)texID, ImVec2(mConfig.ImageSize.x, mConfig.ImageSize.y), ImVec2(0, 0), ImVec2(1, 1), 0);

		if (ImGui::IsItemHovered() == true && 
			ImGui::IsMouseDown(ImGuiMouseButton_Left) == true)
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
			if ((mConfig.Flags & EEditorUISceneFlags::Fix_RotatePitch) == false)
			{
				mModelRotation.x += Math::ConvertToRadians(delta.y);
			}
			if ((mConfig.Flags & EEditorUISceneFlags::Fix_RotateYaw) == false)
			{
				mModelRotation.y += Math::ConvertToRadians(delta.x);
			}
			
			JG_LOG_INFO("Image Drag  {0} , {1} ", delta.x, delta.y);
		}
		else
		{
			mMousePosition = JVector2(-1, -1);
			mPrevMousePosition = JVector2(-1, -1);
		}
		UpdateScene();
		UpdateModel();
		UpdateLight();
		PushRenderObject();
		Rendering();
	}

	void EditorUIScene::SetModel(SharedPtr<Graphics::SceneObject> model)
	{
		mConfig.Model = model;
	}

	void EditorUIScene::SetLocation(const JVector3& location)
	{
		mConfig.OffsetLocation = location;
	}

	void EditorUIScene::SetScale(const JVector3& scale)
	{
		mConfig.OffsetScale = scale;
	}

	void EditorUIScene::SetTargetVector(const JVector3& targetVec)
	{
		mConfig.TargetVec = JVector3::Normalize(targetVec);
	}

	void EditorUIScene::Init()
	{
		UpdateScene();


	}

	void EditorUIScene::UpdateScene()
	{
		const JVector3 upVec(0, 1, 0);


		Graphics::SceneInfo sceneInfo;
		sceneInfo.RenderPath = ERendererPath::RayTracing;
		sceneInfo.Resolution = mConfig.Resolution;
		sceneInfo.EyePos     = mConfig.EyePos;
		sceneInfo.ViewMatrix = JMatrix::LookAtLH(mConfig.EyePos, mConfig.TargetVec, upVec);
		sceneInfo.ProjMatrix = JMatrix::PerspectiveFovLH(Math::ConvertToRadians(90), mConfig.Resolution.x / mConfig.Resolution.y, mConfig.NearZ, mConfig.FarZ);
		sceneInfo.ViewProjMatrix = sceneInfo.ViewMatrix * sceneInfo.ProjMatrix;
		sceneInfo.NearZ = mConfig.NearZ;
		sceneInfo.FarZ  = mConfig.FarZ;
		sceneInfo.ClearColor = Color();
		if (mScene == nullptr)
		{
			mScene = JGGraphics::GetInstance().CreateScene("ModelView_Scene", sceneInfo);
		}
		mScene->SetSceneInfo(sceneInfo);
	}

	void EditorUIScene::UpdateModel()
	{
		if (mConfig.Model)
		{
			JQuaternion q = JQuaternion::RotationRollPitchYawFromVector(JVector3(mModelRotation.x, mModelRotation.y, 0.0f));
			JMatrix S = JMatrix::Scaling(mConfig.OffsetScale);
			JMatrix R = JMatrix::Rotation(q);
			JMatrix T = JMatrix::Translation(mConfig.OffsetLocation);

			mConfig.Model->WorldMatrix = S * R * T;
		}
	}

	void EditorUIScene::UpdateLight()
	{
		if (mLight == nullptr)
		{
			SharedPtr<Graphics::DirectionalLight> dl = CreateSharedPtr<Graphics::DirectionalLight>();
			dl->Color = Color::White();
			dl->Direction = JVector3(-0.2f, -0.8f, 1.0f);
			dl->Intensity = 1.0f;
			mLight = dl;
		}

	}

	void EditorUIScene::PushRenderObject()
	{
		if (mScene->IsEnableRendering())
		{
			mScene->PushSceneObject(mConfig.Model);
			mScene->PushSceneObject(mConfig.SkyBox);
			mScene->PushLight(mLight);
		}
	}

	void EditorUIScene::Rendering()
	{
		if (mScene == nullptr)
		{
			return;
		}
		SharedPtr<Graphics::SceneResultInfo> resultInfo = mScene->FetchResult();
		if (resultInfo != nullptr && resultInfo->Texture != nullptr && resultInfo->Texture->IsValid() == true)
		{
			mSceneTexture = resultInfo->Texture;
		}
		mScene->Rendering();
	}
}