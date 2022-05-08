#include "pch.h"
#include "MaterialView.h"
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
	MaterialView::MaterialView()
	{
		DisableUniqueView();
		

	}
	void MaterialView::Load()
	{
		
	}
	void MaterialView::Initialize()
	{
		String enginePath = Application::GetEnginePath();
		mMeshAsset = AssetDataBase::GetInstance().LoadOriginAsset<IMesh>(PathHelper::CombinePath(enginePath, "Mesh/Sphere.jgasset"));
		UpdateScene();
		
	}
	void MaterialView::OnGUI()
	{
		mWindowHeight = ImGui::GetWindowHeight();
		UpdateScene();
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
		mMaterialAsset = nullptr;
		mTextEditor = nullptr;
		mEditorUIScene = nullptr;
	}
	void MaterialView::OnEvent(IEvent& e)
	{


	}
	void MaterialView::LeftTopOnGUI()
	{
		u64 texID = 0;


		JVector2 padding = JVector2(ImGui::GetStyle().FramePadding.x * 2, ImGui::GetStyle().FramePadding.y * 2);
		JVector2 winSize = mResolution + padding;

		ImGui::BeginChild("LeftTop_OnGUI", ImVec2(winSize.x, winSize.y));
		if (mEditorUIScene != nullptr)
		{
			mEditorUIScene->OnGUI();
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

	void MaterialView::UpdateScene()
	{
		const JVector3 eyePos(0, 0.0f, -200.0f);
		const f32 farZ = 1000000.0f;
		const f32 nearZ = 1.0f;
		if (mEditorUIScene == nullptr)
		{
			EditorUISceneConfig config;
			config.Resolution = mResolution;
			config.ImageSize = mImageSize;
			config.OffsetScale = JVector3(1.5f, 1.5f, 1.5f);
			config.Model = mModel;
			config.SkyBox = GraphicsHelper::CreateSkyBox(eyePos, farZ, "Asset/Engine/CubeMap/DefaultSky.jgasset");;
			config.FarZ = farZ;
			config.NearZ = nearZ;
			config.EyePos = eyePos;
			config.MinZoomDistance = 100.0f;
			mEditorUIScene = CreateUniquePtr<EditorUIScene>(config);
		}
		if (mModel == nullptr)
		{
			mModel = CreateSharedPtr<Graphics::StaticRenderObject>();
		}
		if (mMeshAsset != nullptr && mMeshAsset->IsValid())
		{
			mModel->Mesh = mMeshAsset->Get();
		}
		if (mMaterialAsset != nullptr && mMaterialAsset->IsValid())
		{
			mModel->MaterialList.resize(1);
			mModel->MaterialList[0] = mMaterialAsset->Get();
		}


		mEditorUIScene->SetModel(mModel);
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
		switch (mEditorUIScene->GetScene()->GetRenderer()->GetRendererPath())
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
		mMaterialAsset = AssetDataBase::GetInstance().LoadOriginAsset<IMaterial>(path);
	}

	void MaterialView::SetMesh(const String& path)
	{
		SharedPtr<Asset<IMesh>> asset = AssetDataBase::GetInstance().LoadOriginAsset<IMesh>(path);
		if (asset != nullptr && asset->IsValid())
		{
			mModel->Mesh = asset->Get();
		}
	}
}