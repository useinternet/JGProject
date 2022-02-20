#include "pch.h"
#include "Raytracer.h"
#include "Application.h"
#include "TopLevelAccelerationStructure.h"
#include "BottomLevelAccelerationStructure.h"
#include "RayTracingShaderResourceTable.h"

#include "RayTracingPipeline.h"
#include "RTAO.h"
#include "Denoiser.h"
#include "Graphics/Resource.h"
#include "Graphics/Mesh.h"
#include "Graphics/RootSignature.h"
#include "Graphics/GraphicsHelper.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/Compute/CalculatePartialDerivatives.h"
#include "Graphics/Compute/FloatAccumulater.h"
#include "Graphics/Compute/Blur.h"

namespace JG
{
	void RayTracer::CB::Begin(const Graphics::RenderPassData& passData)
	{
		ProjMatrix = passData.ProjMatrix;
		ViewMatrix = passData.ViewMatrix;
		ViewProjMatrix = passData.ViewProjMatrix;
		InvViewMatrix = passData.InvViewMatrix;
		InvProjMatrix = passData.InvProjMatrix;
		InvViewProjMatrix = passData.InvViewProjMatrix;
		Resolution = passData.Resolution;
		NearZ = passData.NearZ;
		FarZ = passData.FarZ;

		EyePosition = passData.EyePosition;
		PointLightCount = passData.PointLightCount;

		ClusterSize = passData.ClusterSize;
		ClusterScale = passData.ClusterScale;
		ClusterBias = passData.ClusterBias;

		NumClusterSlice = passData.NumClusterSlice;
		FrameCount = passData.FrameCount;


	}

	void RayTracer::CB::End()
	{
		PrevFrameEyePosition	   = EyePosition;
		PrevFrameViewProjMatrix    = ViewProjMatrix;
		PrevFrameInvViewProjMatrix = InvViewProjMatrix;
	}

	RayTracer::RayTracer(Renderer* renderer) : mRenderer(renderer)
	{
		if (JGGraphics::GetInstance().IsSupportedRayTracing() == false)
		{
			return;
		}

		Init();
	}

	void RayTracer::AddInstance(SharedPtr<ISubMesh> subMesh, SharedPtr<IMaterial> material, const List<JMatrix>& transform)
	{
		u64 instanceCount = subMesh->GetInstanceCount();
		JMatrix defaultTransform = transform[0];
		for (u64 i = 0; i < instanceCount; ++i)
		{
			JMatrix m;
			if (transform.size() <= i)
			{
				m = defaultTransform;
			}
			else
			{
				m = transform[i];
			}
			mInstances.push_back(InstanceData(subMesh, material, m, i, mHitGroupOffset));
		}
		mHitGroupOffset += mHitGroupStride;
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
		if (JGGraphics::GetInstance().IsSupportedRayTracing() == false || mInstances.empty() == true)
		{
			return;
		}

		UpdateAccelerationStructure(context);

		static i32 testFrame = 0;
		testFrame++;
		if (testFrame >= 20)
		{
			Update(context);

			for (int i = 0; i < EResource::Count; ++i)
			{
				mTex[i].SetValue(GetResource((EResource)i));
			}

		}
		else
		{
			testFrame++;
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
		mInstances.clear();
		mSceneAS[currentIndex]->Reset();
		mHitGroupOffset = 0;
	}



	void RayTracer::Init()
	{
		GraphicsHelper::InitTopLevelAccelerationStructure(&mSceneAS);
		mRayBounds = RP_Global_Int::Create("Renderer/RayTracing/MaxRayBounds", 3, 1, 5, mRenderer->GetRenderParamManager());	


		mTex[EResource::Result] = RP_Global_Tex::Create("Renderer/Raytracing/Result", nullptr, mRenderer->GetRenderParamManager());
		mTex[EResource::Direct] = RP_Global_Tex::Create("Renderer/Raytracing/Direct", nullptr, mRenderer->GetRenderParamManager());
		mTex[EResource::Indirect] = RP_Global_Tex::Create("Renderer/Raytracing/Indirect", nullptr, mRenderer->GetRenderParamManager());
		mTex[EResource::MotionVector] = RP_Global_Tex::Create("Renderer/Raytracing/MotionVector", nullptr, mRenderer->GetRenderParamManager());
		mTex[EResource::ReprojectedNormalDepth] = RP_Global_Tex::Create("Renderer/Raytracing/ReprojectedNormalDepth", nullptr, mRenderer->GetRenderParamManager());
		mTex[EResource::NormalDepth] = RP_Global_Tex::Create("Renderer/Raytracing/NormalDepth", nullptr, mRenderer->GetRenderParamManager());
		mTex[EResource::Depth] = RP_Global_Tex::Create("Renderer/Raytracing/Depth", nullptr, mRenderer->GetRenderParamManager());
		mTex[EResource::PartialDepthDerivatives] = RP_Global_Tex::Create("Renderer/Raytracing/PartialDepthDerivatives", nullptr, mRenderer->GetRenderParamManager());
		mTex[EResource::HitPosition] = RP_Global_Tex::Create("Renderer/Raytracing/HitPosition", nullptr, mRenderer->GetRenderParamManager());
		mAOTex = RP_Global_Tex::Create("Renderer/Raytracing/AO", nullptr, mRenderer->GetRenderParamManager());


		mCalculatePartialDerivatives = CreateUniquePtr<CalculatePartialDerivatives>(mRenderer);
		mRTAO = CreateSharedPtr<RTAO>(mRenderer);
		mAODesnoiser = CreateSharedPtr<Denoiser>(mRenderer);

	}

	void RayTracer::InitTextures()
	{
		TextureInfo texInfo;
		texInfo.Width	  = std::max<u32>(1, mResolution.x);
		texInfo.Height	  = std::max<u32>(1, mResolution.y);
		texInfo.ArraySize = 1;
		texInfo.Format	  = ETextureFormat::R16G16B16A16_Float;
		texInfo.Flags	  = ETextureFlags::Allow_UnorderedAccessView | ETextureFlags::Allow_RenderTarget;
		texInfo.MipLevel  = 1;
		texInfo.ClearColor = Color();


		GraphicsHelper::InitRenderTextures(texInfo, "DirectOutput", &mResources[EResource::Direct]);
		GraphicsHelper::InitRenderTextures(texInfo, "IndirectOutput", &mResources[EResource::Indirect]);
		GraphicsHelper::InitRenderTextures(texInfo, "ResultOutput", &mResources[EResource::Result]);

		
		texInfo.Format = ETextureFormat::R32_Uint;
		GraphicsHelper::InitRenderTextures(texInfo, "NormalDepthOutput", &mResources[EResource::NormalDepth]);
		GraphicsHelper::InitRenderTextures(texInfo, "ReprojectedNormalDepth", &mResources[EResource::ReprojectedNormalDepth]);


		texInfo.Format = ETextureFormat::R32G32B32A32_Float;
		GraphicsHelper::InitRenderTextures(texInfo, "HitPosition", &mResources[EResource::HitPosition]);

		texInfo.Format = ETextureFormat::R16G16_Float;
		GraphicsHelper::InitRenderTextures(texInfo, "MotionVectorOutput", &mResources[EResource::MotionVector]);
		GraphicsHelper::InitRenderTextures(texInfo, "PartialDepthDerivatives", &mResources[EResource::PartialDepthDerivatives]);

		texInfo.Format = ETextureFormat::R16_Float;
		GraphicsHelper::InitRenderTextures(texInfo, "DepthOutput", &mResources[EResource::Depth]);
		
	}

	void RayTracer::UpdateAccelerationStructure(SharedPtr<IComputeContext> context)
	{
		u32 currentIndex = JGGraphics::GetInstance().GetBufferIndex();
		for (InstanceData& instance : mInstances)
		{
			SharedPtr<IBottomLevelAccelerationStructure> blas = instance.SubMesh->GetBottomLevelAS();

			if (blas == nullptr)
			{
				blas = IBottomLevelAccelerationStructure::Create();
				blas->Generate(context, instance.SubMesh->GetVertexBuffer(), instance.SubMesh->GetIndexBuffer());
				instance.SubMesh->SetBottomLevelAS(blas);
			}

			mSceneAS[currentIndex]->AddInstance(blas, instance.Transform, instance.InstanceID, instance.HitGroupIndex);
		}
		mSceneAS[currentIndex]->Generate(context);
	}
	void RayTracer::Update(SharedPtr<IComputeContext> context)
	{
		if (mSRT == nullptr)
		{
			mSRT = IRayTracingShaderResourceTable::Create();
		}
		mSRT->Reset();
		mSRT->AddRayGeneration(ShaderDefine::RayTracing::RayGen);
		mSRT->AddMiss(ShaderDefine::RayTracing::DirectMiss);
		mSRT->AddMiss(ShaderDefine::RayTracing::IndirectMiss);
		mSRT->AddMiss(ShaderDefine::RayTracing::ShadowMiss);
		for (int i = 0; i < mInstances.size(); ++i)
		{
			InstanceData& instance = mInstances[i];
			LocalRootArgument arg;
			arg.SetIndexBuffer(instance.SubMesh->GetIndexBuffer());
			arg.SetVertexBuffer(instance.SubMesh->GetVertexBuffer());
			if (instance.Material == nullptr || instance.Material->IsValid() == false)
			{
				mSRT->AddHitGroupAndBindLocalRootArgument(ShaderDefine::RayTracing::DirectNullHitGroup, arg);
				mSRT->AddHitGroupAndBindLocalRootArgument(ShaderDefine::RayTracing::IndirectNullHitGroup, arg);
				mSRT->AddHitGroupAndBindLocalRootArgument(ShaderDefine::RayTracing::ShadowHitGroup);
			}
			else
			{
				const List<jbyte>& cbData = instance.Material->GetMaterialPropertyByteData();
				arg.SetConstant(cbData.data(), cbData.size());

				const List<SharedPtr<ITexture>>& texes = instance.Material->GetTextureList();
				arg.SetTextures(texes);


				String scriptName = instance.Material->GetScript()->GetName();
				auto directHitShader   = ShaderLibrary::GetInstance().FindClosestHitShader(ShaderDefine::Template::DirectClosestHitShader, scriptName);
				auto indirectHitShader = ShaderLibrary::GetInstance().FindClosestHitShader(ShaderDefine::Template::IndirectClosestHitShader, scriptName);
				mSRT->AddHitGroupAndBindLocalRootArgument(directHitShader->GetHitGroupName(), arg);
				mSRT->AddHitGroupAndBindLocalRootArgument(indirectHitShader->GetHitGroupName(), arg);
				mSRT->AddHitGroupAndBindLocalRootArgument(ShaderDefine::RayTracing::ShadowHitGroup);
			}


		}
		u32 currentIndex = JGGraphics::GetInstance().GetBufferIndex();
		SharedPtr<IRayTracingPipeline> pipeline = ShaderLibrary::GetInstance().FindRayTracingPipeline(GetDefaultRayTracingPipelineName());
		pipeline->Generate();

		context->BindRootSignature(pipeline->GetGlobalRootSignature());


		mCB.Begin(mRenderer->GetPassData());
		mCB.MaxRayDepth = mRayBounds.GetValue();
		// CB Bind
		context->BindConstantBuffer(0, mCB);

		// SceneAS
		context->BindAccelerationStructure(1, mSceneAS[currentIndex]);

		// Result
		context->BindTextures(2, {
			GetResource(EResource::Direct), 
			GetResource(EResource::Indirect),
			GetResource(EResource::Result),
			GetResource(EResource::MotionVector),
			GetResource(EResource::ReprojectedNormalDepth),
			GetResource(EResource::NormalDepth),
			GetResource(EResource::Depth),
			GetResource(EResource::HitPosition)});
		
		// PointLight
		const auto& plInfo = mRenderer->GetLightInfo(Graphics::ELightType::PointLight);
		context->BindSturcturedBuffer(3, plInfo.Data.data(), plInfo.Size, plInfo.Count);

		// LightGrid
		context->BindSturcturedBuffer(4, mRenderer->GetLightGrid());
		// VisibleLightIndicies
		context->BindSturcturedBuffer(5, mRenderer->GetVisibleLightIndicies());
		// PrevFrameTransform
		context->BindSturcturedBuffer(6, mSceneAS[currentIndex]->GetPrevFrameTransformBuffer());



		context->DispatchRay(mResolution.x, mResolution.y, 1, pipeline, mSRT);

		mCB.End();


		if (mCalculatePartialDerivatives != nullptr)
		{
			CalculatePartialDerivatives::Input input;
			input.Resolution = mCB.Resolution;
			input.Input = GetResource(EResource::Depth);
			input.Output = GetResource(EResource::PartialDepthDerivatives);
			mCalculatePartialDerivatives->Execute(context, input);
		}



		// RTAO
		SharedPtr<ITexture> AOResult;
		SharedPtr<ITexture> AOHitDistance;
		f32 MaxAORayHitDistance = 0.0f;
		{
			RTAO::Input input;
			input.Resolution = mResolution;
			input.NormalDepth = GetResource(EResource::NormalDepth);
			input.HitPosition = GetResource(EResource::HitPosition);
			input.SceneAS = mSceneAS[currentIndex];
			RTAO::Output output = mRTAO->Execute(context, input);

			AOResult = output.AO;
			AOHitDistance = output.AORayDistance;
			MaxAORayHitDistance = output.MaxRayDistance;
		}


		// Denoise(±¸Çö Áß)
		{
			Denoiser::Input input;
			input.Resolution = mResolution;
			input.NormalDepth = GetResource(EResource::NormalDepth);
			input.DepthDerivatives = GetResource(EResource::PartialDepthDerivatives);
			input.ReprojectedNormalDepth = GetResource(EResource::ReprojectedNormalDepth);
			input.MotionVector		= GetResource(EResource::MotionVector);
			input.Value = AOResult;
			input.RayHitDistance = AOHitDistance;
			input.MaxRayDistance = MaxAORayHitDistance;
			input.Depth = GetResource(EResource::Depth);
			Denoiser::Output output = mAODesnoiser->Execute(context, input);
			mAOTex.SetValue(output.OutValue);
			
		}


	}

	SharedPtr<ITexture> RayTracer::GetResource(EResource type)
	{
		return mResources[type][JGGraphics::GetInstance().GetBufferIndex()];
	}

	const String& RayTracer::GetDefaultRayTracingPipelineName()
	{
		static String result = "DefaultRayTracingPipeline";
		return result;
	}

	u64 RayTracer::GetMaxConstantBufferSize()
	{
		return 256;
	}

	SharedPtr<IRootSignature> RayTracer::CreateGlobalRootSignature()
	{
		SharedPtr<IRootSignatureCreater> creater = IRootSignatureCreater::Create();
		creater->AddCBV(0, 0, 0);
		creater->AddSRV(1, 0, 0);
		creater->AddDescriptorTable(2, EDescriptorTableRangeType::UAV, EResource::Count, 0, 0);
		creater->AddSRV(3, 1, 0);
		creater->AddSRV(4, 2, 0);
		creater->AddSRV(5, 3, 0);
		creater->AddSRV(6, 4, 0);

		creater->AddSampler(0, ESamplerFilter::Point, ETextureAddressMode::Wrap);
		creater->AddSampler(1, ESamplerFilter::Linear, ETextureAddressMode::Wrap);
		creater->AddSampler(2, ESamplerFilter::Anisotropic, ETextureAddressMode::Wrap);
		creater->AddSampler(3, ESamplerFilter::Point, ETextureAddressMode::Clamp);
		creater->AddSampler(4, ESamplerFilter::Linear, ETextureAddressMode::Clamp);
		creater->AddSampler(5, ESamplerFilter::Anisotropic, ETextureAddressMode::Clamp);
		creater->AddSampler(6, ESamplerFilter::Linear, ETextureAddressMode::Border);
		return creater->Generate();
	}

	SharedPtr<IRootSignature> RayTracer::CreateLocalRootSignature()
	{
		SharedPtr<IRootSignatureCreater> creater = IRootSignatureCreater::Create();
		creater->AddConstant(0, GetMaxConstantBufferSize(), 0, 1);
		creater->AddDescriptorTable(1, EDescriptorTableRangeType::SRV, 1024, 0, 1);
		return creater->Generate(true);
	}



}

