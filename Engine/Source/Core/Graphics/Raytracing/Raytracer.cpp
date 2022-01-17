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


		context->BindRootSignature(mGlobalRootSignature);
		// Output Texture 바인딩
		context->BindTextures(0, { mOutputTextures[currentIndex] });
		// Top - Level AS 바인딩
		context->BindAccelerationStructure(1, mSceneAS[currentIndex]);
		context->BindConstantBuffer(2, _CB);
		context->DispatchRay(mResolution.x, mResolution.y, 1, mPipeline);
		mShadowTex.SetValue(mOutputTextures[currentIndex]);
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
		if (mRenderer == nullptr)
		{
			return;
		}
		const RenderInfo& info = mRenderer->GetRenderInfo();
		_CB._ViewMatrix = JMatrix::Transpose(info.ViewMatrix);
		_CB._ProjMatrix = JMatrix::Transpose(info.ProjMatrix);
		_CB._InverseViewMatrix = JMatrix::Transpose(JMatrix::Inverse(info.ViewMatrix));
		_CB._InverseProjMatrix = JMatrix::Transpose(JMatrix::Inverse(info.ProjMatrix));
		_CB._EyePosition = info.EyePosition;
		_CB._FarZ = info.FarZ;
		_CB._NearZ = info.NearZ;
		_CB._PointLightCount = 0;


		//mRenderer->GetLightInfo(Graphics::ELightType)



		//for (int x = 0; x < 1920; ++x)
		//{
		//	for (int y = 0; y < 1080; ++y)
		//	{
		//		JVector2 launchIndex = JVector2(x, y);
		//		JVector2 dimensions = JVector2(1920, 1080);

		//		f32 ndcX = ((launchIndex.x + 0.5f) / dimensions.x) * 2.0f - 1.0f;
		//		f32 ndcY = ((launchIndex.y + 0.5f) / dimensions.y) * 2.0f - 1.0f;


		//		JVector4 target = _CB._InverseProjMatrix.Transform(JVector4(ndcX, -ndcY, 1, 1));//mul(_InverseProjMatrix, float4(ndc.x, -ndc.y, 100, 1));
		//		JVector3 direciton = _CB._InverseViewMatrix.TransformVector(JVector3(target.x, target.y, target.z)); 


		//		int n = 0;
		//	}
		//}
		

	}

	void RayTracer::Init()
	{
		InitRootSignature();

		GraphicsHelper::InitTopLevelAccelerationStructure(&mSceneAS);

		mPipeline = IRayTracingPipeline::Create();




		mPipeline->AddLibrary(SHADER_PATH("StandardRayTracingShader.hlsli"), 
			{"RayGeneration", "ClosestHit", "Miss"});
		mPipeline->AddHitGroup("HitGroup0", "ClosestHit", "", "");



		mPipeline->AddRayGenerationProgram("RayGeneration");
		mPipeline->AddHitProgram("HitGroup0");
		mPipeline->AddMissProgram("Miss");
		mPipeline->SetGlobalRootSignature(mGlobalRootSignature);
		mPipeline->SetMaxPayloadSize(sizeof(Color));
		mPipeline->SetMaxAttributeSize(sizeof(float[2]));
		mPipeline->SetMaxRecursionDepth(1);
		if (mPipeline->Generate() == false)
		{
			JG_CORE_ERROR("Failed RayTracing Pipline");
		}

		mShadowTex = RP_Global_Tex::Create("RayTracing/Shadow", nullptr, mRenderer->GetRenderParamManager());
	}

	void RayTracer::InitTextures()
	{
		TextureInfo texInfo;
		texInfo.Width	  = std::max<u32>(1, mResolution.x);
		texInfo.Height	  = std::max<u32>(1, mResolution.y);
		texInfo.ArraySize = 1;
		texInfo.Format	  = ETextureFormat::R8G8B8A8_Unorm;
		texInfo.Flags	  = ETextureFlags::Allow_UnorderedAccessView;
		texInfo.MipLevel  = 1;
		texInfo.ClearColor = Color();

		GraphicsHelper::InitRenderTextures(texInfo, "RayTacingOutput", &mOutputTextures);




	}
	
	void RayTracer::InitRootSignature()
	{
		SharedPtr<IRootSignatureCreater> creater = IRootSignatureCreater::Create();
		creater->AddDescriptorTable(0, EDescriptorTableRangeType::UAV, 1024, 0, 0);
		creater->AddSRV(1, 0, 0);
		creater->AddCBV(2, 0, 0);
		mGlobalRootSignature = creater->Generate();
	}
}

