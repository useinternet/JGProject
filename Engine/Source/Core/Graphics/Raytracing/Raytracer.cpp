#include "pch.h"
#include "Raytracer.h"
#include "Application.h"
#include "TopLevelAccelerationStructure.h"
#include "BottomLevelAccelerationStructure.h"
#include "RayTracingPipeline.h"
#include "Graphics/Resource.h"
#include "Graphics/Mesh.h"
#include "Graphics/RootSignature.h"
#include "Graphics/GraphicsHelper.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/Compute/FloatAccumulater.h"
#include "Graphics/Compute/Blur.h"

#define SHADER_PATH(x) PathHelper::CombinePath(PathHelper::CombinePath(Application::GetEnginePath(), "Shader/Raytracing"),x)

namespace JG
{
	RayTracer::RayTracer(Renderer* renderer) : mRenderer(renderer)
	{
		if (JGGraphics::GetInstance().IsSupportedRayTracing() == false)
		{
			return;
		}

		Init();


	}

	void RayTracer::AddInstance(SharedPtr<IBottomLevelAccelerationStructure> btas, const JMatrix& transform, u32 instanceID, u32 hitGroupIndex)
	{
		if (mSceneAS.empty() == true)
		{
			return;
		}

		u32 currentIndex = JGGraphics::GetInstance().GetGraphicsAPI()->GetBufferIndex();
		if (mSceneAS[currentIndex] == nullptr)
		{
			return;
		}
		mSceneAS[currentIndex]->AddInstance(btas, transform, instanceID, hitGroupIndex);
	}
	void RayTracer::SetResolution(const JVector2& resolutoin)
	{
		if (mResolution != resolutoin)
		{
			mResolution = resolutoin;
			InitTextures();
		}
	}
	void RayTracer::Execute(SharedPtr<IComputeContext> context)
	{
		if (JGGraphics::GetInstance().IsSupportedRayTracing() == false)
		{
			return;
		}

		UpdateAccelerationStructure(context);
		UpdateShadow(context);
	}

	void RayTracer::Reset()
	{
		if (mSceneAS.empty() == true)
		{
			return;
		}

		u32 currentIndex = JGGraphics::GetInstance().GetGraphicsAPI()->GetBufferIndex();
		if (mSceneAS[currentIndex] == nullptr)
		{
			return;
		}

		mSceneAS[currentIndex]->Reset();
	}



	void RayTracer::Init()
	{
		GraphicsHelper::InitTopLevelAccelerationStructure(&mSceneAS);
		BuildShadow();
	}

	void RayTracer::InitTextures()
	{
		TextureInfo texInfo;
		texInfo.Width	  = std::max<u32>(1, mResolution.x);
		texInfo.Height	  = std::max<u32>(1, mResolution.y);
		texInfo.ArraySize = 1;
		texInfo.Format	  = ETextureFormat::R32_Float;
		texInfo.Flags	  = ETextureFlags::Allow_UnorderedAccessView;
		texInfo.MipLevel  = 1;
		texInfo.ClearColor = Color();

		texInfo.Format = ETextureFormat::R32_Float;
		GraphicsHelper::InitRenderTextures(texInfo, "ShadowOutput", &mShadow.Results);


		texInfo.Format = ETextureFormat::R8G8B8A8_Unorm;
		GraphicsHelper::InitRenderTextures(texInfo, "ReflectOutput", &mReflection.Results);

	}
	void RayTracer::BuildShadow()
	{
		mShadow.Result = RP_Global_Tex::Create("Renderer/Raytracing/Shadow", nullptr, mRenderer->GetRenderParamManager());

		// RootSignature
		auto rootSigCreater = IRootSignatureCreater::Create();
		rootSigCreater->AddCBV(0, 0, 0);
		rootSigCreater->AddSRV(1, 0, 1);
		rootSigCreater->AddSRV(2, 0, 2);
		rootSigCreater->AddDescriptorTable(3, EDescriptorTableRangeType::SRV, 1, 0, 0);
		rootSigCreater->AddDescriptorTable(4, EDescriptorTableRangeType::UAV, 1, 0, 0);
		mShadow.RootSignature = rootSigCreater->Generate();



		// Pipeline
		mShadow.Pipeline = IRayTracingPipeline::Create();
		mShadow.Pipeline->AddLibrary(SHADER_PATH("Shadow.hlsli"), { "RayGeneration", "ClosestHit", "Miss" });
		mShadow.Pipeline->AddHitGroup("ShadowHitGroup", "ClosestHit", "", "");

		mShadow.Pipeline->AddRayGenerationProgram("RayGeneration");
		mShadow.Pipeline->AddHitProgram("ShadowHitGroup");
		mShadow.Pipeline->AddMissProgram("Miss");
		mShadow.Pipeline->SetGlobalRootSignature(mShadow.RootSignature);
		mShadow.Pipeline->SetMaxPayloadSize(sizeof(float));
		mShadow.Pipeline->SetMaxAttributeSize(sizeof(float[2]));
		mShadow.Pipeline->SetMaxRecursionDepth(1);
		mShadow.Pipeline->Generate();
	}
	void RayTracer::BuildReflection()
	{
		mReflection.Result = RP_Global_Tex::Create("Renderer/Raytracing/Reflect", nullptr, mRenderer->GetRenderParamManager());
		


	}
	void RayTracer::BuildGlobalIllumination()
	{
	}
	void RayTracer::UpdateAccelerationStructure(SharedPtr<IComputeContext> context)
	{
		u32 currentIndex = JGGraphics::GetInstance().GetBufferIndex();
		mSceneAS[currentIndex]->Generate(context);


	}
	void RayTracer::UpdateShadow(SharedPtr<IComputeContext> context)
	{
		struct CB
		{
			int PointLightCount = 0;
		};
		static CB CB;

		u32 currentIndex = JGGraphics::GetInstance().GetBufferIndex();

		SharedPtr<ITexture> worldPosTex = RP_Global_Tex::Load("Renderer/GBuffer/WorldPos", mRenderer->GetRenderParamManager()).GetValue();

		auto lInfo = mRenderer->GetLightInfo(Graphics::ELightType::PointLight);


		CB.PointLightCount = lInfo.Count;



		context->BindRootSignature(mShadow.RootSignature);

		// CB Bind
		context->BindConstantBuffer(0, CB);

		// SceneAS
		context->BindAccelerationStructure(1, mSceneAS[currentIndex]);

		// PointLight
		context->BindSturcturedBuffer(2, lInfo.Data.data(), lInfo.Size, lInfo.Count);

		// WorldPos
		context->BindTextures(3, { worldPosTex } );

		// Result
		context->BindTextures(4, { mShadow.Results[currentIndex] });



		context->DispatchRay(mResolution.x, mResolution.y, 1, mShadow.Pipeline);

		mShadow.Result.SetValue(mShadow.Results[currentIndex]);



	}
	void RayTracer::UpdateReflection(SharedPtr<IComputeContext> context)
	{
	}
	void RayTracer::UpdateGlobalIllumination(SharedPtr<IComputeContext> context)
	{
	}
}

