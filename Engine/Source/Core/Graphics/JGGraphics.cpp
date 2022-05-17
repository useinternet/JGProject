#include "pch.h"
#include "JGGraphics.h"
#include "Class/Global/GlobalSingletonManager.h"
#include "RayTracing/RayTracingPipeline.h"
#include "RayTracing/Raytracer.h"
#include "Graphics/RootSignature.h"
#include "Graphics/DebugGeometryDrawer.h"
#include "Graphics/Batch/Render2DBatch.h"
#include "Graphics/Renderer/FowardRenderer.h"
#include "Graphics/Renderer/DeferredRenderer.h"
#include "Graphics/Renderer/RayTracingRenderer.h"

#include "GraphicsHelper.h"
#include "Graphics/Develop/RenderStatistics.h"
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
		Reset();
		mObjectPool.clear();
		GlobalSingletonManager::GetInstance().UnRegisterSingleton<ShaderLibrary>();

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
	u64 JGGraphics::GetBufferIndex() const
	{
		if (mGraphcisAPI == nullptr)
		{
			return 0;
		}
		return mGraphcisAPI->GetBufferIndex();
	}
	bool JGGraphics::IsSupportedRayTracing() const
	{
		return mGraphcisAPI->IsSupportedRayTracing();
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
			mGraphcisAPI->Flush(GRAPHICS_COMMAND_QUEUE_ID);
			mGraphcisAPI->BeginFrame();
			Reset();
			
			return EScheduleResult::Continue;
		});

		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::Graphics_EndFrame,
			[&]() -> EScheduleResult
		{
			if (mGraphcisAPI == nullptr)
			{
				return EScheduleResult::Break;
			}

			mGraphcisAPI->EndFrame();
			

			return EScheduleResult::Continue;
		});

		mGraphcisAPI->AllocateCommandQueue(ECommandQueueType::Graphics, GRAPHICS_COMMAND_QUEUE_ID);
		GlobalSingletonManager::GetInstance().RegisterSingleton<ShaderLibrary>();
	}
	void JGGraphics::Reset()
	{
		// Statics Reset
		Renderer::Statistics.TotalObjectCount   = 0;
		Renderer::Statistics.CullingObjectCount = 0;
		Renderer::Statistics.VisibleObjectCount = 0;
	}
	void JGGraphics::LoadShader()
	{
		ShaderLibrary::GetInstance().LoadGlobalShaderLib(mDesc.GlobalShaderLibPath);
		LoadRayTracingPipeline();
		LoadShaderTemplate();
		LoadShaderScript();
		LoadComputeShader();
	}
	void JGGraphics::LoadShaderTemplate()
	{
		auto templatePath = Application::GetShaderTemplatePath();
		for (auto& iter : fs::recursive_directory_iterator(templatePath))
		{
			auto p = iter.path();
			if (p.extension() != SHADER_TEMPLATE_FORMAT)
			{
				continue;
			}
			auto fileName = StringHelper::ReplaceAll(p.filename().string(), p.extension().string(), "");
			EShaderFlags shaderFlags = EShaderFlags::None;

			std::ifstream fin(p.string());
			String sourceCode;
			FileHelper::ReadAllText(p.string(), &sourceCode);
			if (sourceCode.empty() == false)
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

				if (shaderFlags != EShaderFlags::None)
				{
					auto shader = IGraphicsShader::Create(fileName, sourceCode, shaderFlags);
					ShaderLibrary::GetInstance().RegisterGraphicsShader(fileName, shader);
				}
				else
				{
					if (sourceCode.find(HLSL::ClosestHitEntry) != String::npos)
					{
						auto shader = IClosestHitShader::Create(fileName, sourceCode, nullptr);
						ShaderLibrary::GetInstance().RegisterClosestHitShader(fileName, shader);
					}
					else if (sourceCode.find(HLSL::MissEntry) != String::npos)
					{
						auto shader = IClosestHitShader::Create(fileName, sourceCode, nullptr);
						ShaderLibrary::GetInstance().RegisterClosestHitShader(fileName, shader);
					}
				}

			}

		}
	}
	void JGGraphics::LoadShaderScript()
	{
		// Shader Script
		auto scriptPath = Application::GetShaderScriptPath();
		for (auto& iter : fs::recursive_directory_iterator(scriptPath))
		{
			auto p = iter.path();

			if (p.extension() != SHADER_SCRIPT_FORMAT)
			{
				continue;
			}
			auto fileName = StringHelper::ReplaceAll(p.filename().string(), p.extension().string(), "");

			SharedPtr<IShaderScript> script;
			String scriptCode;

			FileHelper::ReadAllText(p.string(), &scriptCode);
			if (scriptCode.find(ShaderDefine::Type::Surface) != String::npos)
			{
				script = IShaderScript::CreateSurfaceScript("Surface/" + fileName, scriptCode);

			}
			else if (scriptCode.find(ShaderDefine::Type::Scene) != String::npos)
			{
				script = IShaderScript::CreateSceneScript("Scene/" + fileName, scriptCode);
			}
			ShaderLibrary::GetInstance().RegisterShaderScript(script->GetName(), script);
		}
	}
	void JGGraphics::LoadComputeShader()
	{
		// Load ComputeShader
		auto computeShaderPath = Application::GetComputeShaderScriptPath();
		for (auto& iter : fs::recursive_directory_iterator(computeShaderPath))
		{
			auto p = iter.path();

			if (p.extension() != COMPUTE_SHADER_FORMAT)
			{
				continue;
			}
			auto fileName = StringHelper::ReplaceAll(p.filename().string(), p.extension().string(), "");
			String sourceCode;
			FileHelper::ReadAllText(p.string(), &sourceCode);
			if (sourceCode.empty() == false)
			{
				if (sourceCode.find(HLSL::CSEntry) != String::npos)
				{
					auto computeShader = IComputeShader::Create(fileName, sourceCode);
					ShaderLibrary::GetInstance().RegisterComputeShader(fileName, computeShader);
				}
			}
		}
	}
	void JGGraphics::LoadRayTracingPipeline()
	{
		if (IsSupportedRayTracing() == false)
		{
			return;
		}
		SharedPtr<IRayTracingPipeline> pipeline = IRayTracingPipeline::Create();
		//Pipeline
		String shaderPath = PathHelper::CombinePath(Application::GetEnginePath(), "Shader/Raytracing");
		pipeline = IRayTracingPipeline::Create();
		pipeline->AddLibrary(PathHelper::CombinePath(shaderPath, "RayTracer.hlsli"), 
			{ ShaderDefine::RayTracing::RayGen, 
			  ShaderDefine::RayTracing::DirectNullHit,
			  ShaderDefine::RayTracing::DirectMiss,
			  ShaderDefine::RayTracing::IndirectNullHit,
			  ShaderDefine::RayTracing::IndirectMiss,
			  ShaderDefine::RayTracing::ShadowHit,
			  ShaderDefine::RayTracing::ShadowMiss});

		pipeline->AddHitGroup(
			ShaderDefine::RayTracing::DirectNullHitGroup,
			ShaderDefine::RayTracing::DirectNullHit,
			"", "");
		pipeline->AddHitGroup(
			ShaderDefine::RayTracing::IndirectNullHitGroup,
			ShaderDefine::RayTracing::IndirectNullHit,
			"", "");
		pipeline->AddHitGroup(
			ShaderDefine::RayTracing::ShadowHitGroup,
			ShaderDefine::RayTracing::ShadowHit,
			"","");
		pipeline->SetGlobalRootSignature(RayTracer::CreateGlobalRootSignature());
		pipeline->AddLocalRootSignature(RayTracer::CreateLocalRootSignature(), { ShaderDefine::RayTracing::DirectNullHit });
		pipeline->AddLocalRootSignature(RayTracer::CreateLocalRootSignature(), { ShaderDefine::RayTracing::IndirectNullHit });
		pipeline->SetMaxPayloadSize(sizeof(float[64]));
		pipeline->SetMaxRecursionDepth(10);
		pipeline->Generate();

		ShaderLibrary::GetInstance().RegisterRayTracingPipeline(RayTracer::GetDefaultRayTracingPipelineName(), pipeline);
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

		const String& GObject::GetName()
		{
			return mName;
		}

		void GObject::Lock()
		{
			mLock = true;
		}

		void GObject::UnLock()
		{
			mLock = false;
		}

		bool GObject::IsLock() const
		{
			return mLock;
		}

		Scene::Scene(const SceneInfo& info)
		{
			SetSceneInfo(info);
		}
		Scene::~Scene()
		{
			if (mRenderSH != nullptr)
			{
				mRenderSH->Reset();
				mRenderSH = nullptr;
			}
			if (mRenderFetchResultSH != nullptr)
			{
				mRenderFetchResultSH->Reset();
				mRenderFetchResultSH = nullptr;
			}
			if (mRenderInternalSH != nullptr)
			{
				mRenderInternalSH->Reset();
				mRenderInternalSH = nullptr;
			}
		}
		void Scene::SetSceneInfo(const SceneInfo& info)
		{
			mPendingSceneInfo = info;
			Scheduler::GetInstance().ScheduleOnceByFrame(0, SchedulePriority::Graphics_BeginFrame, [&]() -> EScheduleResult
			{
				if (mRenderer == nullptr || mSceneInfo.RenderPath != mPendingSceneInfo.RenderPath)
				{
					InitRenderer(mPendingSceneInfo.RenderPath);
				}
				if (mRenderSH == nullptr)
				{
					mRenderSH = Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::Graphics_Rendering, SCHEDULE_BIND_FN(&Scene::RenderScene));
				}
				if (mRenderFetchResultSH == nullptr)
				{
					mRenderFetchResultSH = Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::Graphics_RenderFinish, SCHEDULE_BIND_FN(&Scene::RenderFinish));
				}

				mSceneInfo = mPendingSceneInfo;
				return EScheduleResult::Break;
			});
		}

		const SceneInfo& Scene::GetSceneInfo() const
		{
			return mSceneInfo;
		}

		Renderer* Scene::GetRenderer() const
		{
			return mRenderer.get();
		}

		bool Scene::PushSceneObject(SharedPtr<SceneObject> sceneObject)
		{
			if (mRenderer == nullptr || sceneObject == nullptr)
			{
				return false;
			}
			mPendingSceneObjectQueue.push(sceneObject);
			return true;
		}

		bool Scene::PushLight(SharedPtr<Light> l)
		{
			if (mRenderer == nullptr || l == nullptr)
			{
				return false;
			}
			mPendingLightList.push_back(l);
			return true;
		}

		void Scene::PushPostRenderingEvent(i32 priority, const PostRenderingEvent& _e)
		{
			mPostRenderingEventQueue[priority].push(_e);
		}


		void Scene::Reset()
		{
			mSceneResult = nullptr;
			while (mSceneObjectQueue.empty() == false) {
				mSceneObjectQueue.pop();
			}
			mLightList.clear();
			mPostRenderingEventQueue.clear();
		}

		void Scene::Rendering()
		{
			if (mRenderer == nullptr || IsLock() == true)
			{
				return;
			}
			Lock();
		}

		bool Scene::IsEnableRendering() const
		{
			if (IsLock() == true)
			{
				return false;
			}
			return true;
		}

		// 완료 될때까지 기다렷다가, SceneResultInfo 리턴
		SharedPtr<SceneResultInfo> Scene::FetchResult()
		{
			//
			if (IsLock() == false)
			{
				return mSceneResult;
			}
			else
			{
				return nullptr;
			}
		}


		EScheduleResult Scene::RenderScene()
		{
			if (IsLock() == false)
			{
				return EScheduleResult::Continue;
			}
			// Scene 정보 수정을 할수 없게 Lock
			mSceneObjectQueue = std::move(mPendingSceneObjectQueue);
			mLightList		  = std::move(mPendingLightList);


			// 비동기로 렌더링 시작
			mRenderInternalSH = Scheduler::GetInstance().ScheduleAsync([&]()
			{
				u32 bufferCnt = JGGraphics::GetInstance().GetGraphicsAPI()->GetBufferCount();
				RenderInfo info;
				info.Resolution = mSceneInfo.Resolution;
				info.ViewProjMatrix = mSceneInfo.ViewProjMatrix;
				info.ViewMatrix = mSceneInfo.ViewMatrix;
				info.ProjMatrix = mSceneInfo.ProjMatrix;
				info.FarZ  = mSceneInfo.FarZ;
				info.NearZ = mSceneInfo.NearZ;
				info.EyePosition = mSceneInfo.EyePos;
				info.ClearColor  = mSceneInfo.ClearColor;
				info.AmbientColor = mSceneInfo.AmbientColor;

				if (mRenderer->Begin(info, mLightList) == true)
				{
					while (mSceneObjectQueue.empty() == false)
					{
						auto obj = mSceneObjectQueue.front(); mSceneObjectQueue.pop();
						mRenderer->DrawCall(obj);
					}

					auto result = mRenderer->End();
					if (result != nullptr)
					{
						mSceneResult = CreateSharedPtr<SceneResultInfo>();
						mSceneResult->Texture = result->SceneTexture;

						for (auto& _pair : mPostRenderingEventQueue)
						{
							while (_pair.second.empty() == false)
							{
								auto func = _pair.second.front(); _pair.second.pop();
								if (func != nullptr)
								{
									func(mSceneResult);
								}
							}
						}
					}
					else {
						mSceneResult = nullptr;
					}

					mPostRenderingEventQueue.clear();
					mLightList.clear();
				}
			});

			return EScheduleResult::Continue;
		}

		EScheduleResult Scene::RenderFinish()
		{
			if (IsLock() == false || mRenderInternalSH == nullptr)
			{
				return EScheduleResult::Continue;
			}
			// Rendering이 끝날때까지 기다린다.
			while (mRenderInternalSH->GetState() != EScheduleState::Compelete)
			{
				//
			}

			mRenderInternalSH->Reset();
			mRenderInternalSH = nullptr;
			// Rendering 이 끝났으니. 다시 정보수정 Ok
			UnLock();
			return EScheduleResult::Continue;
		}


		void Scene::InitRenderer(ERendererPath path)
		{
			auto bufferCount = JGGraphics::GetInstance().GetGraphicsAPI()->GetBufferCount();
			switch (path)
			{
			//case ERendererPath::Deferred:
				//mRenderer = CreateSharedPtr<DeferredRenderer>();
			case ERendererPath::RayTracing:
				mRenderer = CreateSharedPtr<RayTracingRenderer>();
				break;
			}
		}
	}
}

