#include "pch.h"
#include "MaterialView.h"
#include "Application.h"
#include "ExternalImpl/JGImGui.h"
#include "ExternalImpl/TextEditor.h"
#include "Class/Asset/Asset.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/GraphicsHelper.h"
#include "UI/UIManager.h"

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
		mWindowHeight = ImGui::GetWindowHeight();

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
		mTextEditor = nullptr;
		JGGraphics::GetInstance().DestroyObject(mScene);
		mScene = nullptr;
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

		UIManager::GetInstance().ShowInspectorUI(mMaterialAsset.get());

		ImGui::EndChild();
	}
	void MaterialView::RightTopOnGUI()
	{
		if (mTextEditor == nullptr && mMaterialAsset != nullptr && mMaterialAsset->IsValid())
		{
			auto script = mMaterialAsset->Get()->GetScript();
			if (script != nullptr)
			{
				LoadTextEditor(script->GetCode());
			}
		}
		f32 winHeight = std::max<f32>(300.0f, mWindowHeight - mRightBottomHeight);
		ImGui::BeginChild("RightTopOnGUI", ImVec2(0, winHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
		if (mTextEditor != nullptr && mMaterialAsset != nullptr && mMaterialAsset->IsValid())
		{
			auto cpos = mTextEditor->GetCursorPosition();
			if (ImGui::IsWindowFocused())
			{
				if (ImGui::IsKeyDown((i32)EKeyCode::Ctrl) && ImGui::IsKeyPressed((i32)EKeyCode::C))
				{
					mTextEditor->Copy();
				}
				if (ImGui::IsKeyDown((i32)EKeyCode::Ctrl) && ImGui::IsKeyPressed((i32)EKeyCode::X))
				{
					mTextEditor->Cut();
				}
				if (ImGui::IsKeyDown((i32)EKeyCode::Ctrl) && ImGui::IsKeyPressed((i32)EKeyCode::V))
				{
					mTextEditor->Paste();
				}
				if (ImGui::IsKeyDown((i32)EKeyCode::Ctrl) && ImGui::IsKeyPressed((i32)EKeyCode::A))
				{
					mTextEditor->SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(mTextEditor->GetTotalLines(), 0));
				}
			}
			String scriptName;
			SharedPtr<IShaderScript> script = mMaterialAsset->Get()->GetScript();
			if (script != nullptr)
			{
				scriptName = script->GetName();
			}

			ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, mTextEditor->GetTotalLines(),
				mTextEditor->IsOverwrite() ? "Ovr" : "Ins",
				mTextEditor->CanUndo() ? "*" : " ",
				mTextEditor->GetLanguageDefinition().mName.c_str(), scriptName.c_str());

			mTextEditor->Render(mMaterialAsset->GetAssetPath().c_str());
	
		}
		ImGui::EndChild();

	}
	void MaterialView::RightBottomOnGUI()
	{
		ImGui::BeginChild("RightBottom_OnGUI");
		
		if (ImGui::Button("Save") == true)
		{
			SaveAndApplyScript();

		} ImGui::SameLine();
		if (ImGui::Button("Compile") == true)
		{
			CompileShader();
		} ImGui::SameLine();
		if (ImGui::Button("Clear") == true)
		{
			ClearLogs();
		}
		ImGui::Separator();
		ImGui::BeginChild("ErrorLogChildWindow", ImVec2(0, 150.0f), true);
		for (const String& log : mMaterialLogs)
		{
			ImGui::Text(log.c_str());
		}

		ImGui::EndChild();
		ImGui::EndChild();
	}
	void MaterialView::LoadTextEditor(const String& text)
	{
		mTextEditor = CreateUniquePtr<TextEditor>();
		mTextEditor->SetLanguageDefinition(TextEditor::LanguageDefinition::HLSL());
		mTextEditor->SetText(text);
		mTextEditor->SetPalette(TextEditor::GetDarkPalette());
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

	void MaterialView::SaveAndApplyScript()
	{
		if (CompileShader() == false)
		{
			AddLog("Failed Save Material");
			return;
		}

		SharedPtr<IShaderScript> script = mMaterialAsset->Get()->GetScript();
		String scriptPath = PathHelper::CombinePath(Application::GetInstance().GetShaderScriptPath(), "Surface");
		scriptPath = PathHelper::CombinePath(scriptPath, script->GetName() + SHADER_SCRIPT_FORMAT);
	
		if (FileHelper::WriteAllText(scriptPath, mTextEditor->GetText()) == false)
		{
			JG_LOG_ERROR("Failed Save Script : {0}", scriptPath);
		}
	}
	bool MaterialView::CompileShader()
	{
		ClearLogs();
		if (mTextEditor == nullptr)
		{
			AddLog("TextEditor is null");
			return false;
		}
		if (mMaterialAsset == nullptr || mMaterialAsset->IsValid() == false)
		{
			AddLog("Material Asset is null or InValid");
			return false;
		}
		SharedPtr<IShaderScript> script = mMaterialAsset->Get()->GetScript();
		if (script == nullptr)
		{
			AddLog("Material Script is null");
			return false;
		}
		bool mIsCompileSuccess = false;
		SharedPtr<IShaderScript> newScript = IShaderScript::CreateSurfaceScript(script->GetName(), mTextEditor->GetText());
		switch (mScene->GetRenderer()->GetRendererPath())
		{
		case ERendererPath::RayTracing:
		{
			SharedPtr<IClosestHitShader> directShader = ShaderLibrary::GetInstance().FindClosestHitShader(ShaderDefine::Template::DirectClosestHitShader);
			SharedPtr<IClosestHitShader> newShader	  = IClosestHitShader::Create("TempDirectShader", directShader->GetShaderCode(), newScript);
			mIsCompileSuccess = newShader != nullptr && newShader->IsSuccessed();
			if (mIsCompileSuccess == false)
			{
				AddLog(newShader->GetErrorMessage());
			}
		}
			break;
		default:
			AddLog("Current Not Supported RendererPath");
			return false;
		}
		return mIsCompileSuccess;
	}
	void MaterialView::AddLog(const String& log)
	{
		mMaterialLogs.push_back("Error : " + log);
	}
	void MaterialView::ClearLogs()
	{
		mMaterialLogs.clear();
	}
	void MaterialView::SetMaterial(const String& path)
	{
		//AssetID originMaterial = AssetDataBase::GetInstance().GetAssetOriginID(path);
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