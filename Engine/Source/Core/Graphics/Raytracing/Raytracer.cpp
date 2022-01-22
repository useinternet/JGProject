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
		UpdateCB();

		u32 currentIndex = JGGraphics::GetInstance().GetGraphicsAPI()->GetBufferIndex();

		mSceneAS[currentIndex]->Generate(context);


		auto lInfo = mRenderer->GetLightInfo(Graphics::ELightType::PointLight);



		context->BindRootSignature(mGlobalRootSignature);
		// Output Texture 바인딩
		context->BindTextures(RootParam_Output, { mOutputTextures[currentIndex] });
		// Top - Level AS 바인딩
		context->BindAccelerationStructure(RootParam_SceneAS, mSceneAS[currentIndex]);
		context->BindConstantBuffer(RootParam_CB, _CB);
		context->BindSturcturedBuffer(RootParam_PointLightSB, lInfo.SB[currentIndex]);

		context->DispatchRay(mResolution.x, mResolution.y, 1, mPipeline);

		mShadowTex.SetValue(mOutputTextures[currentIndex]);
		{
			//Compute::Blur_Float::Input input;
			//input.Resolution = mResolution;
			//input.TextureFormat = ETextureFormat::R32_Float;
			//input.Src = mShadowTex.GetValue();
			//Compute::Blur_Float::Output output = mShaowBlur->Execute(context, input);
			//mShadowTex.SetValue(output.Result);
		}
		{
			Compute::FloatAccumulater::Input input;
			input.CurrFrame = mShadowTex.GetValue();
			input.IsResetAccumCount = mIsResetAccumCount;
			input.Resolution = mResolution;
			Compute::FloatAccumulater::Output output = mShadowAccumulater->Execute(context, input);
			mShadowTex.SetValue(output.Result);

			if (mIsResetAccumCount)
			{
				mIsResetAccumCount = false;
			}
		}

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

	void RayTracer::UpdateCB()
	{
		u32 currentIndex = JGGraphics::GetInstance().GetGraphicsAPI()->GetBufferIndex();
		if (mRenderer == nullptr)
		{
			return;
		}
		auto plInfo = mRenderer->GetLightInfo(Graphics::ELightType::PointLight);
		const RenderInfo& info = mRenderer->GetRenderInfo();


		{
			JMatrix viewMatrix = JMatrix::Transpose(info.ViewMatrix);
			for (i32 i = 0; i < 4; ++i)
			{
				for (i32 j = 0; j < 4; ++j)
				{
					if (_CB.ViewMatrix.Get_C(i, j) != viewMatrix.Get_C(i, j))
					{
						mIsResetAccumCount = true;
					}
				}
			}
		}

		_CB.ViewMatrix = JMatrix::Transpose(info.ViewMatrix);
		_CB.ProjMatrix = JMatrix::Transpose(info.ProjMatrix);
		_CB.InverseViewMatrix = JMatrix::Transpose(JMatrix::Inverse(info.ViewMatrix));
		_CB.InverseProjMatrix = JMatrix::Transpose(JMatrix::Inverse(info.ProjMatrix));
		_CB.EyePosition = info.EyePosition;
		_CB.FarZ = info.FarZ;
		_CB.NearZ = info.NearZ;
		_CB.PointLightCount = plInfo.OriginCount[currentIndex];


		static u32 seedFrameCount = 0;
		_CB.FrameCount = seedFrameCount++;
	}

	void RayTracer::Init()
	{
		InitRootSignature();
		GraphicsHelper::InitTopLevelAccelerationStructure(&mSceneAS);

		mPipeline = IRayTracingPipeline::Create();




		mPipeline->AddLibrary(SHADER_PATH("StandardRayTracingShader.hlsli"), 
			{"RayGeneration", "ClosestHit", "ShadowHit","Miss", "ShadowMiss"});
		mPipeline->AddHitGroup("FirstHitGroup", "ClosestHit", "", "");
		mPipeline->AddHitGroup("ShadowHitGroup", "ShadowHit", "", "");

		mPipeline->AddRayGenerationProgram("RayGeneration");
		mPipeline->AddHitProgram("FirstHitGroup");
		mPipeline->AddHitProgram("ShadowHitGroup");
		mPipeline->AddMissProgram("Miss");
		mPipeline->AddMissProgram("ShadowMiss");
		mPipeline->SetGlobalRootSignature(mGlobalRootSignature);
		mPipeline->SetMaxPayloadSize(sizeof(float[5]));
		mPipeline->SetMaxAttributeSize(sizeof(float[2]));
		mPipeline->SetMaxRecursionDepth(2);
		if (mPipeline->Generate() == false)
		{
			JG_CORE_ERROR("Failed RayTracing Pipline");
		}

		mShadowTex = RP_Global_Tex::Create("RayTracing/Shadow", nullptr, mRenderer->GetRenderParamManager());


		mShadowAccumulater = CreateUniquePtr<Compute::FloatAccumulater>(mRenderer);
		mShaowBlur = CreateUniquePtr<Compute::Blur_Float>(mRenderer);
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

		GraphicsHelper::InitRenderTextures(texInfo, "RayTacingOutput", &mOutputTextures);




	}
	
	void RayTracer::InitRootSignature()
	{
		SharedPtr<IRootSignatureCreater> creater = IRootSignatureCreater::Create();
		creater->AddDescriptorTable(RootParam_Output, EDescriptorTableRangeType::UAV, 1024, 0, 0);
		creater->AddSRV(RootParam_SceneAS, 0, 0);
		creater->AddCBV(RootParam_CB, 0, 0);
		creater->AddSRV(RootParam_PointLightSB, 0, 1);

		mGlobalRootSignature = creater->Generate();
	}
}

