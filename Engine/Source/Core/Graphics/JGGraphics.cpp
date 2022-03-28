#include "JGGraphics.h"
#include "pch.h"
#include "JGGraphics.h"
#include "RayTracing/RayTracingPipeline.h"
#include "RayTracing/Raytracer.h"
#include "Graphics/RootSignature.h"
#include "Graphics/DebugGeometryDrawer.h"
#include "Graphics/Batch/Render2DBatch.h"
#include "Graphics/Renderer/FowardRenderer.h"
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
		ShaderLibrary::Create();
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
			if (p.extension() != ".shadertemplate")
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

			if (p.extension() != ".shaderscript")
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

			if (p.extension() != ".computeshader")
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

		bool GObject::IsLock()
		{
			return mLock;
		}

		Scene::Scene(const SceneInfo& info)
		{
			static u64 s_CommandIDOffset = 0;
			m2DBatch = CreateSharedPtr<Render2DBatch>();
			SetSceneInfo(info);

		}
		Scene::~Scene()
		{

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
			mSceneInfo = info;
			return true;
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
			if (mRenderer == nullptr || m2DBatch == nullptr)
			{
				return;
			}
			if (mRenderScheduleHandle != nullptr || IsLock())
			{
				JG_LOG_ERROR("It's still being rendered.");
				return;
			}
			// Scene 정보 수정을 할수 없게 Lock
			Lock();
			// 비동기로 렌더링 시작
			mRenderScheduleHandle = Scheduler::GetInstance().ScheduleAsync([&]()
			{
				u32 bufferCnt = JGGraphics::GetInstance().GetGraphicsAPI()->GetBufferCount();

				RenderInfo info;
				info.Resolution = mSceneInfo.Resolution;
				info.ViewProjMatrix	    = mSceneInfo.ViewProjMatrix;
				info.ViewMatrix = mSceneInfo.ViewMatrix;
				info.ProjMatrix = mSceneInfo.ProjMatrix;
				info.FarZ  = mSceneInfo.FarZ;
				info.NearZ = mSceneInfo.NearZ;
				info.EyePosition = mSceneInfo.EyePos;
				info.ClearColor  = mSceneInfo.ClearColor;
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
							//auto debugObj = static_cast<DebugRenderObject*>(obj.get());
							//mRenderer->DrawCall(debugObj->WorldMatrix, debugObj->Mesh, debugObj->MaterialList);
						}
							break;
						case ESceneObjectType::Static:
							auto staticObj = static_cast<StaticRenderObject*>(obj.get());
							mRenderer->DrawCall(staticObj->WorldMatrix, staticObj->Mesh, staticObj->MaterialList, staticObj->Flags);
							break;
						// Skeletal
						}

					}

					auto result = mRenderer->End();
					if (result != nullptr)
					{
						if (mSceneResult == nullptr)
						{
							mSceneResult = CreateSharedPtr<SceneResultInfo>();
						}
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

		}

		// 완료 될때까지 기다렷다가, SceneResultInfo 리턴
		SharedPtr<SceneResultInfo> Scene::FetchResultFinish()
		{
			//
			if (mRenderScheduleHandle == nullptr)
			{
				return nullptr;
			}
			// Rendering이 끝날때까지 기다린다.
			while (mRenderScheduleHandle->GetState() != EScheduleState::Compelete)
			{
				//
			}

			mRenderScheduleHandle->Reset();
			mRenderScheduleHandle = nullptr;
			// Rendering 이 끝났으니. 다시 정보수정 Ok
			UnLock();
			return mSceneResult;
		}


		void Scene::InitRenderer(ERendererPath path)
		{
			auto bufferCount = JGGraphics::GetInstance().GetGraphicsAPI()->GetBufferCount();
			switch (path)
			{
			case ERendererPath::Foward:
				mRenderer = CreateSharedPtr<FowardRenderer>();
				break;
			}
		}
	}
}

