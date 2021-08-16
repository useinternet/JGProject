#include "pch.h"
#include "GraphicsSystemLayer.h"
#include "Application.h"
#include "Graphics/GraphicsDefine.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Resource.h"
#include "Graphics/GraphicsAPI.h"
#include "Class/Game/GameSettings.h"
#include "Class/Game/Components/Camera.h"
#include "Class/Asset/AssetImporter.h"
#include "Class/Asset/Asset.h"
#include "Imgui/ImGuizmo.h"


namespace JG
{
	GraphicsSystemLayer::CameraItem::CameraItem(Camera* cam) : pCamera(cam)
	{
		auto bufferCnt = Application::GetInstance().GetGraphicsAPI()->GetBufferCount();
		TargetTextures.resize(bufferCnt, nullptr);
		TargetDepthTextures.resize(bufferCnt, nullptr);
		CurrentIndex = 0;

		_2DBatch = CreateSharedPtr<Render2DBatch>();
	}

	bool GraphicsSystemLayer::CameraItem::IsValid() const
	{
		return TargetTextures[0] != nullptr;
	}

	void GraphicsSystemLayer::CameraItem::ChangeRenderer()
	{
		if (pCamera == nullptr) return;
		if (Renderer != nullptr && pCamera->GetRendererPath() == Renderer->GetRendererPath())
		{
			return;
		}

		auto bufferCnt = Application::GetInstance().GetGraphicsAPI()->GetBufferCount();

		
		if (Renderer != nullptr)
		{
			bool isMakeSchedule = PendingRendererQueue.empty();
			PendingRendererQueue.push(Renderer);

			if (isMakeSchedule)
			{
				Scheduler::GetInstance().ScheduleOnceByFrame(bufferCnt, 0,
					[&]() -> EScheduleResult
				{

					while (!PendingRendererQueue.empty())
					{
						PendingRendererQueue.front().reset();
						PendingRendererQueue.pop();
					}
					return EScheduleResult::Continue;
				});
			}
		}
	

		switch (pCamera->GetRendererPath())
		{
		case ERendererPath::Foward:
			Renderer = CreateSharedPtr<FowardRenderer>();
			break;
		case ERendererPath::Deferred:
			break;
		}

	}


	void GraphicsSystemLayer::OnAttach()
	{
		LoadShaderScript();
	}

	void GraphicsSystemLayer::OnDetach()
	{



	}

	void GraphicsSystemLayer::Begin()
	{
		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::BeginSystem, SCHEDULE_BIND_FN(&GraphicsSystemLayer::Update));
		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::EndSystem, SCHEDULE_BIND_FN(&GraphicsSystemLayer::Wait));

		mRenderItemPriority[JGTYPE(Standard2DRenderItem)] = (u64)ERenderItemPriority::_2D;


		String rawAssetPath = CombinePath(Application::GetAssetPath(), "RawResources");
		String outputPath = CombinePath(Application::GetAssetPath(), "Resources");


		for (auto& iter : fs::recursive_directory_iterator(rawAssetPath))
		{
			auto extenstion = iter.path().extension().string();
			if (extenstion == ".fbx")
			{
				FBXAssetImportSettings settings;
				settings.AssetPath = iter.path().string();
				settings.OutputPath = outputPath;
				auto result = AssetImporter::Import(settings);
				if (result == EAssetImportResult::Success)
				{
					JG_CORE_INFO("Success Import {0}", iter.path().string());
				}
				else
				{
					JG_CORE_INFO("Fail Import {0}", iter.path().string());
				}
			}
			if (extenstion == ".png" || extenstion == ".jpg" || extenstion == ".TGA")
			{
				TextureAssetImportSettings settings;
				settings.AssetPath = iter.path().string();
				settings.OutputPath = outputPath;
				auto result = AssetImporter::Import(settings);
				if (result == EAssetImportResult::Success)
				{
					JG_CORE_INFO("Success Import {0}", iter.path().string());
				}
				else
				{
					JG_CORE_INFO("Fail Import {0}", iter.path().string());
				}
			}
		}
		for (auto& iter : fs::recursive_directory_iterator(outputPath))
		{
			AssetDataBase::GetInstance().LoadOriginAsset(iter.path().string());
		}
	}

	void GraphicsSystemLayer::Destroy()
	{
		mRegisteredRenderCameras.clear();
		mPushedRenderItems.clear();
	}

	void GraphicsSystemLayer::OnEvent(IEvent& e)
	{
		EventDispatcher eventDispatcher(e);
		eventDispatcher.Dispatch<RequestPushRenderItemEvent>(EVENT_BIND_FN(&GraphicsSystemLayer::ResponsePushRenderItem));
		eventDispatcher.Dispatch<RequestRegisterCameraEvent>(EVENT_BIND_FN(&GraphicsSystemLayer::ResponseRegisterCamera));
		eventDispatcher.Dispatch<RequestUnRegisterCameraEvent>(EVENT_BIND_FN(&GraphicsSystemLayer::ResponseUnRegisterCamera));
		eventDispatcher.Dispatch<RequestRegisterMainCameraEvent>(EVENT_BIND_FN(&GraphicsSystemLayer::ResponseRegisterMainCamera));
		eventDispatcher.Dispatch<RequestUnRegisterMainCameraEvent>(EVENT_BIND_FN(&GraphicsSystemLayer::ResponseUnRegisterMainCamera));
	}

	String GraphicsSystemLayer::GetLayerName()
	{
		return "GraphicsSystemLayer";
	}

	bool GraphicsSystemLayer::ResponsePushRenderItem(RequestPushRenderItemEvent& e)
	{
		if (e.RenderItem == nullptr)
		{
			return true;
		}
		auto type     = e.RenderItem->GetType();
		auto priority = mRenderItemPriority[type];
		mPushedRenderItems[(u64)priority][type].push_back(e.RenderItem);

		return true;
	}


	bool GraphicsSystemLayer::ResponseRegisterCamera(RequestRegisterCameraEvent& e)
	{
		if (mRegisteredRenderCameras.find(e.pCamera) != mRegisteredRenderCameras.end())
		{
			return true;
		}
		
		mRegisteredRenderCameras[e.pCamera] = CreateUniquePtr<CameraItem>(e.pCamera);

		return true;
	}

	bool GraphicsSystemLayer::ResponseUnRegisterCamera(RequestUnRegisterCameraEvent& e)
	{
		if (mRegisteredRenderCameras.find(e.pCamera) == mRegisteredRenderCameras.end())
		{
			return true;
		}

		mRegisteredRenderCameras.erase(e.pCamera);
		return true;
	}

	bool GraphicsSystemLayer::ResponseRegisterMainCamera(RequestRegisterMainCameraEvent& e)
	{
		if (mMainCamera == nullptr)
		{
			mMainCamera = CreateUniquePtr<CameraItem>(e.MainCamera);
		}
		else
		{
			mMainCamera->pCamera = e.MainCamera;
		}
		return true;
	}

	bool GraphicsSystemLayer::ResponseUnRegisterMainCamera(RequestUnRegisterMainCameraEvent& e)
	{
		if (mMainCamera->pCamera == e.MainCamera)
		{
			mMainCamera->pCamera = nullptr;
		}
		return true;
	}

	void GraphicsSystemLayer::Rendering(CameraItem* cameraItem, Type type, const List<SharedPtr<IRenderItem>>& renderItemList)
	{
		if (type == JGTYPE(Standard2DRenderItem))
		{
			u64 layerMask = cameraItem->pCamera->GetCullingLayerMask();
			
			for (auto& item : renderItemList)
			{
				auto itemMask = GameLayer::GetMask(item->TargetLayer);
				if ((itemMask & layerMask) == false) continue;

				auto _2dItem = static_cast<Standard2DRenderItem*>(item.get());
				cameraItem->_2DBatch->DrawCall(_2dItem->WorldMatrix, _2dItem->Texture, _2dItem->Color);
			}
		}
		else if (type == JGTYPE(StandardStaticMeshRenderItem))
		{
			u64 layerMask = cameraItem->pCamera->GetCullingLayerMask();
			for (auto& item : renderItemList)
			{
				auto itemMask = GameLayer::GetMask(item->TargetLayer);
				if ((itemMask & layerMask) == false) continue;

				auto _3dItem = static_cast<StandardStaticMeshRenderItem*>(item.get());
				if (_3dItem->Materials.empty())
				{
					_3dItem->Materials.push_back(IMaterial::Create("NullMaterial", ShaderLibrary::GetInstance().GetShader(ShaderScript::Template::Standard3DShader)));
				}
				cameraItem->Renderer->DrawCall(_3dItem->WorldMatrix, _3dItem->Mesh, _3dItem->Materials);
			}
		}
		else
		{
			// Not Supported
		}
	}
	EScheduleResult GraphicsSystemLayer::Update()
	{
		{
			NotifyRenderingReadyCompeleteEvent e;
			Application::GetInstance().SendEvent(e);
		}
		if (mMainCamera == nullptr)
		{
			return EScheduleResult::Continue;
		}
		bool isResize = false;
		if (mMainCamera->pCamera != nullptr && mMainCamera->Resolution != mMainCamera->pCamera->GetResolution())
		{
			isResize = true;
			mMainCamera->IsResizeDirty = true;
			mMainCamera->Resolution = mMainCamera->pCamera->GetResolution();
		}
		for (auto& _pair : mRegisteredRenderCameras)
		{
			auto& camItem = _pair.second;
			if (camItem->Resolution != camItem->pCamera->GetResolution())
			{
				isResize = true;
				camItem->IsResizeDirty = true;
				camItem->Resolution = camItem->pCamera->GetResolution();
			}
		}
		if (isResize == true)
		{
			JG_CORE_INFO("Graphics API Flush");
			Application::GetInstance().GetGraphicsAPI()->Flush();
		}
		auto fmBufferCnt = Application::GetInstance().GetGraphicsAPI()->GetBufferCount();
		if (mMainCamera->pCamera != nullptr)
		{
			auto mainCam = mMainCamera->pCamera;
			if (mMainCamera->IsValid() == false)
			{
				TextureInfo mainTexInfo;
				mainTexInfo.Width = std::max<u32>(1, mMainCamera->Resolution.x);
				mainTexInfo.Height = std::max<u32>(1, mMainCamera->Resolution.y);
				mainTexInfo.ArraySize = 1;
				mainTexInfo.Format = ETextureFormat::R8G8B8A8_Unorm;
				mainTexInfo.Flags = ETextureFlags::Allow_RenderTarget;
				mainTexInfo.MipLevel = 1;
				mainTexInfo.ClearColor = mainCam->GetClearColor();

				for (auto& t : mMainCamera->TargetTextures)
				{
					t = ITexture::Create(mainCam->GetName() + "TargetTexture", mainTexInfo);
				}

				mainTexInfo.Format = ETextureFormat::D24_Unorm_S8_Uint;
				mainTexInfo.Flags = ETextureFlags::Allow_DepthStencil;
				for (auto& t : mMainCamera->TargetDepthTextures)
				{
					t = ITexture::Create(mainCam->GetName() + "TargetDepthTexture", mainTexInfo);
				}
			}
		}

		RenderInfo info; auto mainCam = mMainCamera->pCamera;
		info.CurrentBufferIndex = mMainCamera->CurrentIndex;
		info.Resolutoin			= mMainCamera->pCamera->GetResolution();
		info.ViewProj = mMainCamera->pCamera->GetViewProjMatrix();
		info.TargetTexture		= mMainCamera->TargetTextures[info.CurrentBufferIndex];
		info.TargetDepthTexture = mMainCamera->TargetDepthTextures[info.CurrentBufferIndex];
		info.TargetTexture->SetClearColor(mainCam->GetClearColor());
		mMainCamera->ChangeRenderer();

		mIsRenderCompelete = false;
		Scheduler::GetInstance().ScheduleAsync([&](void* data)
		{
			mMainCamera->Renderer->SetCommandID(1);
			if (mMainCamera->Renderer->Begin(*(RenderInfo*)data, { mMainCamera->_2DBatch }) == true)
			{
				for (auto& _pair : mPushedRenderItems)
				{
					for (auto& itemPair : _pair.second)
					{
						auto& type = itemPair.first;
						auto& itemList = itemPair.second;
						Rendering(mMainCamera.get(), type, itemList);
						itemList.clear();
					}
				}
				mMainCamera->Renderer->End();
				mIsRenderCompelete = true;
			}
		}, & info, sizeof(RenderInfo));

		mMainCamera->CurrentIndex = (mMainCamera->CurrentIndex + 1) % fmBufferCnt;
		{
			NotifyChangeMainSceneTextureEvent e;
			e.SceneTexture = mMainCamera->TargetTextures[mMainCamera->CurrentIndex];
			Application::GetInstance().SendEvent(e);
		}


		return EScheduleResult::Continue;
	}
	EScheduleResult GraphicsSystemLayer::Wait()
	{
		while (mIsRenderCompelete == false) {}
		if (mIsRenderCompelete == true)
		{
			mPushedRenderItems.clear();
		}

		return EScheduleResult::Continue;
	}
	void GraphicsSystemLayer::LoadShaderScript()
	{
		auto templatePath = Application::GetShaderTemplatePath();

		for (auto& iter : fs::recursive_directory_iterator(templatePath))
		{
			auto p = iter.path();
			if (p.extension() != ".shadertemplate")
			{
				continue;
			}
			auto fileName = ReplaceAll(p.filename().string(), p.extension().string(), "");
			EShaderFlags shaderFlags = EShaderFlags::None;

			std::ifstream fin(p.string());

			if (fin.is_open() == true)
			{
				std::stringstream ss;
				ss << fin.rdbuf();
				String sourceCode = ss.str();
				if (sourceCode.find(HLSL::CSEntry) != String::npos)
				{
					shaderFlags = shaderFlags | EShaderFlags::Allow_ComputeShader;
				}
				else
				{
					if (sourceCode.find(HLSL::VSEntry) != String::npos)
					{
						shaderFlags = shaderFlags | EShaderFlags::Allow_VertexShader;
					}
					if (sourceCode.find(HLSL::GSEntry) != String::npos)
					{
						shaderFlags = shaderFlags | EShaderFlags::Allow_GeometryShader;
					}
					if (sourceCode.find(HLSL::HSEntry) != String::npos)
					{
						shaderFlags = shaderFlags | EShaderFlags::Allow_HullShader;
					}
					if (sourceCode.find(HLSL::DSEntry) != String::npos)
					{
						shaderFlags = shaderFlags | EShaderFlags::Allow_DomainShader;
					}
					if (sourceCode.find(HLSL::PSEntry) != String::npos)
					{
						shaderFlags = shaderFlags | EShaderFlags::Allow_PixelShader;
					}
				}
				auto shader = IShader::Create(fileName, sourceCode, shaderFlags);
				ShaderLibrary::GetInstance().RegisterShader(shader);
				fin.close();
			}
		}
		auto scriptPath = Application::GetShaderScriptPath();

		for (auto& iter : fs::recursive_directory_iterator(scriptPath))
		{
			auto p = iter.path();

			if (p.extension() != ".shaderscript")
			{
				continue;
			}
			auto fileName = ReplaceAll(p.filename().string(), p.extension().string(), "");
			std::ifstream fin(p.string());

			if (fin.is_open() == true)
			{
				std::stringstream ss;
				ss << fin.rdbuf();
				String scriptCode = ss.str();
				SharedPtr<IShaderScript> script;
				if (scriptCode.find(ShaderScript::Type::Surface) != String::npos)
				{
					script = IShaderScript::CreateMaterialScript("Surface/" + fileName, scriptCode);
				}


				ShaderLibrary::GetInstance().RegisterScirpt(script);
				fin.close();
			}
			
		}
	}

}

