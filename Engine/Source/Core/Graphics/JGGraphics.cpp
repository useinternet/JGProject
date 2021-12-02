#include "pch.h"
#include "JGGraphics.h"
#include "Graphics/DebugGeometryDrawer.h"
#include "Graphics/Batch/Render2DBatch.h"
#include "Graphics/Renderer/FowardRenderer.h"
#include "Graphics/Renderer/DeferredRenderer.h"
#include "Application.h"

namespace JG
{
	JGGraphics::JGGraphics(const JGGraphicsDesc& desc)
	{
		mDesc = desc;

		Init();
	}

	JGGraphics::~JGGraphics()
	{
		Flush();
		mObjectPool.clear();
		ShaderLibrary::Destroy();
		mGraphcisAPI->Destroy();
		mGraphcisAPI.reset();
	}

	Graphics::Scene* JGGraphics::CreateScene(const String& name, const Graphics::SceneInfo& info)
	{
		auto pScene = CreateGObject<Graphics::Scene>(name, info);
		mSceneDic.emplace(pScene, pScene);
		return pScene;
	}
	void JGGraphics::DestroyObject(Graphics::GObject* gobject)
	{
		if (mGraphcisAPI == nullptr)
		{
			return;
		}
		mSceneDic.erase(gobject);



		Scheduler::GetInstance().ScheduleOnceByFrame(mGraphcisAPI->GetBufferCount(), SchedulePriority::Graphics_DestroyObject,
			[&](SharedPtr<IJGObject> userData) -> EScheduleResult
		{

			if (userData->Is<RemoveObjectData>())
			{
				RemoveObject(userData->As<RemoveObjectData>()->GObject);
			}
			return EScheduleResult::Continue;
		}, CreateSharedPtr<RemoveObjectData>(gobject));
	}

	void JGGraphics::ForEach(const std::function<void(Graphics::Scene*)>& action)
	{
		for (auto& _pair : mSceneDic)
		{
			auto scene = _pair.second;
			if (scene == nullptr)
			{
				continue;
			}
			if (action != nullptr)
			{
				action(scene);
			}
		}

	}

	DebugGeometryDrawer* JGGraphics::GetDebugGeometryDrawer() const
	{
		return mDebugGeometryDrawer.get();
	}

	IGraphicsAPI* JGGraphics::GetGraphicsAPI() const
	{
		return mGraphcisAPI.get();
	}

	u64 JGGraphics::GetBufferCount() const
	{
		if (mGraphcisAPI == nullptr)
		{
			return 0;
		}
		return mGraphcisAPI->GetBufferCount();
	}

	const JGGraphicsDesc& JGGraphics::GetDesc() const
	{
		return mDesc;
	}

	void JGGraphics::Flush()
	{
		if (mGraphcisAPI != nullptr)
		{
			mGraphcisAPI->Flush();
		}
	}

	void JGGraphics::Init()
	{
		mGraphcisAPI = IGraphicsAPI::Create(mDesc.GraphicsAPI);
		mGraphcisAPI->Create();
		mDebugGeometryDrawer = CreateUniquePtr<DebugGeometryDrawer>();
		
		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::Graphics_BeginFrame,
			[&]() -> EScheduleResult
		{

			if (mGraphcisAPI == nullptr)
			{
				return EScheduleResult::Break;
			}
			mGraphcisAPI->Begin();


			return EScheduleResult::Continue;
		});

		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::Graphics_EndFrame,
			[&]() -> EScheduleResult
		{
			if (mGraphcisAPI == nullptr)
			{
				return EScheduleResult::Break;
			}
			mGraphcisAPI->End();
			return EScheduleResult::Continue;
		});
		ShaderLibrary::Create();
	}

	void JGGraphics::LoadShader()
	{
		ShaderLibrary::GetInstance().LoadGlobalShaderLib(mDesc.GlobalShaderLibPath);
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


namespace JG
{
	namespace Graphics
	{
		void GObject::SetName(const String& name)
		{
			mName = name;
		}

		void GObject::SetLayer(u64 layer)
		{
			mLayer = layer;
		}

		const String& GObject::GetName()
		{
			return mName;
		}

		u64 GObject::GetLayer()
		{
			return mLayer;
		}

		void GObject::Lock()
		{
			mLock = true;
		}

		void GObject::UnLock()
		{
			mLock = false;
		}

		bool GObject::IsLock()
		{
			return mLock;
		}


		Queue<u64> Scene::sm_CommandIDQueue;
		std::mutex Scene::sm_CommandIDMutex;
		Scene::Scene(const SceneInfo& info)
		{
			static u64 s_CommandIDOffset = 0;
			auto bufferCount = JGGraphics::GetInstance().GetGraphicsAPI()->GetBufferCount();
			mTargetTextures.resize(bufferCount, nullptr);
			mTargetDepthTextures.resize(bufferCount, nullptr);
			mCurrentIndex = 0;
			m2DBatch = CreateSharedPtr<Render2DBatch>();


			{
				std::lock_guard<std::mutex> lock(sm_CommandIDMutex);
				if (sm_CommandIDQueue.empty() == false)
				{
					mCommandID = sm_CommandIDQueue.front();
					sm_CommandIDQueue.pop();
				}
				else
				{
					mCommandID = ++s_CommandIDOffset;
				}
			}


			SetSceneInfo(info);

		}
		Scene::~Scene()
		{
			sm_CommandIDQueue.push(mCommandID);
			for (auto& t : mTargetTextures)
			{
				t.reset();
				t = nullptr;
			}
			for (auto& t : mTargetDepthTextures)
			{
				t.reset();
				t = nullptr;
			}
		}
		bool Scene::SetSceneInfo(const SceneInfo& info)
		{
			if (IsLock())
			{
				return false;
			}
			if (mRenderer == nullptr || mSceneInfo.RenderPath != info.RenderPath)
			{
				InitRenderer(info.RenderPath);
			}
			if (mTargetTextures.empty() || mSceneInfo.Resolution != info.Resolution || mSceneInfo.ClearColor != info.ClearColor)
			{
				InitTexture(info.Resolution, info.ClearColor);
			}
			mSceneInfo = info;
			return true;
		}

		const SceneInfo& Scene::GetSceneInfo() const
		{
			return mSceneInfo;
		}

		bool Scene::PushSceneObject(SharedPtr<SceneObject> sceneObject)
		{
			if (mRenderer == nullptr || m2DBatch == nullptr)
			{
				return false;
			}
			if (IsLock() || sceneObject == nullptr)
			{
				return false;
			}
			mSceneObjectQueue.push(sceneObject);

			return true;
		}

		bool Scene::PushLight(SharedPtr<Light> l)
		{
			if (mRenderer == nullptr || m2DBatch == nullptr)
			{
				return false;
			}
			if (IsLock() || l == nullptr)
			{
				return false;
			}
			mLightList.push_back(l);

			return true;
		}


		void Scene::Rendering()
		{
			if (mRenderer == nullptr || m2DBatch == nullptr)
			{
				return;
			}
			if (mRenderScheduleHandle != nullptr || IsLock())
			{
				JG_CORE_ERROR("It's still being rendered.");
				return;
			}
			// Scene 정보 수정을 할수 없게 Lock
			Lock();
			// 비동기로 렌더링 시작
			mRenderScheduleHandle = Scheduler::GetInstance().ScheduleAsync([&]()
			{
				RenderInfo info;
				info.TargetTexture		= mTargetTextures[mCurrentIndex];
				info.TargetDepthTexture = mTargetDepthTextures[mCurrentIndex];
				info.Resolutoin			= mSceneInfo.Resolution;
				info.ViewProj			= mSceneInfo.ViewProjMatrix;
				info.EyePosition		= mSceneInfo.EyePos;
				info.CurrentBufferIndex = mCurrentIndex;
				info.CommandID          = mCommandID;

				if (mRenderer->Begin(info, mLightList, { m2DBatch }) == true)
				{

					while (mSceneObjectQueue.empty() == false)
					{
						auto obj = mSceneObjectQueue.front(); mSceneObjectQueue.pop();
						switch (obj->GetSceneObjectType())
						{
						case ESceneObjectType::Paper:
						{
							auto paperObj = static_cast<PaperObject*>(obj.get());
							m2DBatch->DrawCall(paperObj->WorldMatrix, paperObj->Texture, paperObj->Color);
						}
							break;
						case ESceneObjectType::Debug:
						{
							auto debugObj = static_cast<DebugRenderObject*>(obj.get());
							mRenderer->DrawCall(debugObj->WorldMatrix, debugObj->Mesh, debugObj->MaterialList);
						}
							break;
						case ESceneObjectType::Static:
							auto staticObj = static_cast<StaticRenderObject*>(obj.get());
							mRenderer->DrawCall(staticObj->WorldMatrix, staticObj->Mesh, staticObj->MaterialList);
							break;
						// Skeletal
						}

					}

					mRenderer->End();
				}
			});

		}

		// 완료 될때까지 기다렷다가, SceneResultInfo 리턴
		SharedPtr<SceneResultInfo> Scene::FetchResultFinish()
		{
			//
			if (mRenderScheduleHandle == nullptr)
			{
				return nullptr;
			}
			SharedPtr<SceneResultInfo> result = CreateSharedPtr<SceneResultInfo>();

			// Rendering이 끝날때까지 기다린다.
			while (mRenderScheduleHandle->GetState() != EScheduleState::Compelete)
			{
				//
			}
			mRenderScheduleHandle->Reset();
			mRenderScheduleHandle = nullptr;
			// Rendering 이 끝났으니. 다시 정보수정 Ok
			UnLock();

			result->Texture      = mTargetTextures[mCurrentIndex];
			result->DepthTexture = mTargetDepthTextures[mCurrentIndex];
			mCurrentIndex = (mCurrentIndex + 1) % JGGraphics::GetInstance().GetGraphicsAPI()->GetBufferCount();

			mLightList.clear();
			return result;
		}


		void Scene::InitRenderer(ERendererPath path)
		{
			auto bufferCount = JGGraphics::GetInstance().GetGraphicsAPI()->GetBufferCount();
			switch (path)
			{
			case ERendererPath::Foward:
				mRenderer = CreateSharedPtr<FowardRenderer>();
				break;
			case ERendererPath::Deferred:
				mRenderer = CreateSharedPtr<DeferredRenderer>();
				break;
			}
		}

		void Scene::InitTexture(const JVector2& resolution, const Color& clearColor)
		{
			TextureInfo mainTexInfo;
			mainTexInfo.Width     = std::max<u32>(1, resolution.x);
			mainTexInfo.Height    = std::max<u32>(1, resolution.y);
			mainTexInfo.ArraySize = 1;
			mainTexInfo.Format    = ETextureFormat::R32G32B32A32_Float;
			mainTexInfo.Flags     = ETextureFlags::Allow_RenderTarget;
			mainTexInfo.MipLevel   = 1;
			mainTexInfo.ClearColor = clearColor;

			i32 index = 0;
			for (auto& t : mTargetTextures)
			{
				if (t == nullptr) t = ITexture::Create(GetName() + std::to_string(mCommandID) + "_TargetTexture_" + std::to_string(index), mainTexInfo);
				else t->SetTextureInfo(mainTexInfo);
				++index;

			}

			mainTexInfo.Format = ETextureFormat::D24_Unorm_S8_Uint;
			mainTexInfo.Flags  = ETextureFlags::Allow_DepthStencil;
			for (auto& t : mTargetDepthTextures)
			{
				if (t == nullptr) t = ITexture::Create(GetName() + "_TargetDepthTexture", mainTexInfo);
				else t->SetTextureInfo(mainTexInfo);

			}
		}
	}
}

