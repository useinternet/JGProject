#include "pch.h"
#include "Raytracer.h"
#include "Application.h"
#include "TopLevelAccelerationStructure.h"
#include "BottomLevelAccelerationStructure.h"
#include "RayTracingShaderResourceTable.h"
#include "RayTracingPipeline.h"
#include "Graphics/Resource.h"
#include "Graphics/Mesh.h"
#include "Graphics/RootSignature.h"
#include "Graphics/GraphicsHelper.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/Compute/FloatAccumulater.h"
#include "Graphics/Compute/Blur.h"

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
		Update(context);
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
		mRayBounds = RP_Global_Int::Create("Renderer/RayTracing/MaxRayBounds", 3, 1, 10, mRenderer->GetRenderParamManager());		mResultTex = RP_Global_Tex::Create("Renderer/Raytracing/Result", nullptr, mRenderer->GetRenderParamManager());
		mDirectTex = RP_Global_Tex::Create("Renderer/Raytracing/Direct", nullptr, mRenderer->GetRenderParamManager());
		mIndirectTex = RP_Global_Tex::Create("Renderer/Raytracing/Indirect", nullptr, mRenderer->GetRenderParamManager());
		mShadowTex = RP_Global_Tex::Create("Renderer/Raytracing/Shadow", nullptr, mRenderer->GetRenderParamManager());
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


		struct CB
		{
			Graphics::RenderPassData PassData;
			u32 MaxRayDepth;
		};
		CB CB;
		CB.PassData    = mRenderer->GetPassData();
		CB.MaxRayDepth = mRayBounds.GetValue();
		// CB Bind
		context->BindConstantBuffer(0, CB);

		// SceneAS
		context->BindAccelerationStructure(1, mSceneAS[currentIndex]);

		// Result
		context->BindTextures(2, {
			GetResource(EResource::Direct), 
			GetResource(EResource::Indirect),
			GetResource(EResource::Result) });
		
		// PointLight
		const auto& plInfo = mRenderer->GetLightInfo(Graphics::ELightType::PointLight);
		context->BindSturcturedBuffer(3, plInfo.Data.data(), plInfo.Size, plInfo.Count);

		// LightGrid
		context->BindSturcturedBuffer(4, mRenderer->GetLightGrid());
		// VisibleLightIndicies
		context->BindSturcturedBuffer(5, mRenderer->GetVisibleLightIndicies());

		context->DispatchRay(mResolution.x, mResolution.y, 1, pipeline, mSRT);

		mResultTex.SetValue(GetResource(EResource::Result));
		mDirectTex.SetValue(GetResource(EResource::Direct));
		mIndirectTex.SetValue(GetResource(EResource::Indirect));
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
		creater->AddDescriptorTable(2, EDescriptorTableRangeType::UAV, 3, 0, 0);
		creater->AddSRV(3, 1, 0);
		creater->AddSRV(4, 2, 0);
		creater->AddSRV(5, 3, 0);


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

