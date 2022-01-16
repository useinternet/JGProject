#include "pch.h"
#include "FowardRenderer.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/RootSignature.h"
#include "Graphics/GraphicsHelper.h"
#include "Graphics/RayTracing/RayTracingPipeline.h"
#include "Graphics/RayTracing/TopLevelAccelerationStructure.h"
#include "Graphics/RayTracing/BottomLevelAccelerationStructure.h"
#include "Graphics/PreRenderProcess/PreRenderProcess_ComputeCluster.h"
#include "Graphics/PreRenderProcess/PreRenderProcess_LightCulling.h"

#include "Graphics/PostRenderProcess/PostProcess_Bloom.h"
#include "Graphics/PostRenderProcess/PostProcess_ToneMapping.h"
#include "Graphics/Batch/Render2DBatch.h"
namespace JG
{

	FowardRenderer::FowardRenderer()
	{
		InitProcesses();
		InitGlobalRenderParams();
		InitRayTracing();
	}

	void FowardRenderer::ReadyImpl(IGraphicsAPI* api, Graphics::RenderPassData* renderPassData, const RenderInfo& info)
	{
		if (mRootSignature == nullptr)
		{
			auto creater = api->CreateRootSignatureCreater();
			creater->AddSRV(RootParam_PointLight, 0, 3);
			creater->AddCBV(RootParam_PassCB, 0, 0);
			creater->AddCBV(RootParam_ObjectCB, 1, 0);
			creater->AddCBV(RootParam_MaterialCB, 2, 0);
			creater->AddDescriptorTable(RootParam_Texture2D, EDescriptorTableRangeType::SRV, 1024, 0, 0);
			creater->AddDescriptorTable(RootParam_TextureCube, EDescriptorTableRangeType::SRV, 1024, 0, 1);
			creater->AddSRV(RootParam_LightGrid, 0, 11);
			creater->AddSRV(RootParam_VisibleLightIndicies, 0, 12);
			creater->AddSampler(0, ESamplerFilter::Point, ETextureAddressMode::Wrap);
			creater->AddSampler(1, ESamplerFilter::Linear, ETextureAddressMode::Wrap);
			creater->AddSampler(2, ESamplerFilter::Anisotropic, ETextureAddressMode::Wrap);
			creater->AddSampler(3, ESamplerFilter::Point, ETextureAddressMode::Clamp);
			creater->AddSampler(4, ESamplerFilter::Linear, ETextureAddressMode::Clamp);
			creater->AddSampler(5, ESamplerFilter::Anisotropic, ETextureAddressMode::Clamp);
			creater->AddSampler(6, ESamplerFilter::Linear, ETextureAddressMode::Border);
			mRootSignature = creater->Generate();
		}
		SharedPtr<IGraphicsContext> context = api->GetGraphicsContext();

		context->BindRootSignature(mRootSignature);



		if (mExposureSB.empty())
		{
			GraphicsHelper::InitStrucutredBuffer("Exposure", 8, sizeof(8), &mExposureSB);
			RP_Global_SB::Create("Renderer/Exposure", mExposureSB[info.CurrentBufferIndex], GetRenderParamManager());
		}
		f32 exposure = mExposure.GetValue();
		f32 initialMinLog = mInitialMinLog.GetValue();
		f32 initialMaxLog = mInitialMaxLog.GetValue();
		List<f32> val; val.resize(8);
		val[0] = exposure;
		val[1] = 1.0f / exposure;
		val[2] = exposure;
		val[3] = 0.0f;
		val[4] = initialMinLog;
		val[5] = initialMaxLog;
		val[6] = initialMaxLog - initialMinLog;
		val[7] = 1.0f / (initialMaxLog - initialMinLog);

		mExposureSB[info.CurrentBufferIndex]->SetData(sizeof(f32), 8, val.data());
		RP_Global_SB::Load("Renderer/Exposure", GetRenderParamManager()).SetValue(mExposureSB[info.CurrentBufferIndex]);

		if (mPrevResolution != info.Resolution ||
			mPrevClearColor != info.ClearColor)
		{
			mPrevResolution = info.Resolution;
			mPrevClearColor = info.ClearColor;
			InitTextures(info.Resolution, info.ClearColor);
		}

	
	}

	void FowardRenderer::RenderImpl(IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result)
	{
		auto targetTexture = mTargetTextures[info.CurrentBufferIndex];
		auto targetDepthTexture = mTargetDepthTextures[info.CurrentBufferIndex];

		if (targetTexture == nullptr || targetDepthTexture == nullptr)
		{
			return;
		}
		SharedPtr<IGraphicsContext> context = api->GetGraphicsContext();
		context->SetViewports({ Viewport(info.Resolution.x, info.Resolution.y) });
		context->SetScissorRects({ ScissorRect(0,0, info.Resolution.x,info.Resolution.y) });
		context->ClearRenderTarget({ targetTexture }, targetDepthTexture);
		context->SetRenderTarget({ targetTexture }, targetDepthTexture);

		ForEach([&](int objectType, const List<ObjectInfo>& objectList)
		{
			for (auto& info : objectList)
			{
				auto mesh = info.Mesh;
				auto& materialList = info.MaterialList;
				auto& worldMatrix  = JMatrix::Transpose(info.WorldMatrix);

				context->BindConstantBuffer(RootParam_ObjectCB, worldMatrix);
				context->DrawIndexedAfterBindMeshAndMaterial(mesh, materialList);
			}
		});
		if (result != nullptr)
		{
			result->SceneTexture = mTargetTextures[info.CurrentBufferIndex];
		}

	}

	void FowardRenderer::CompeleteImpl(IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result)
	{


	}

	int FowardRenderer::ArrangeObject(const ObjectInfo& info)
	{
		return 0;
	}

	void FowardRenderer::InitTextures(const JVector2& size, const Color& clearColor)
	{
		TextureInfo mainTexInfo;
		mainTexInfo.Width  = std::max<u32>(1, size.x);
		mainTexInfo.Height = std::max<u32>(1, size.y);
		mainTexInfo.ArraySize = 1;
		mainTexInfo.Format = ETextureFormat::R32G32B32A32_Float;
		mainTexInfo.Flags = ETextureFlags::Allow_RenderTarget;
		mainTexInfo.MipLevel = 1;
		mainTexInfo.ClearColor = clearColor;


		GraphicsHelper::InitRenderTextures(mainTexInfo, "Foward_TargetTexture", &mTargetTextures);

		mainTexInfo.Format = ETextureFormat::D24_Unorm_S8_Uint;
		mainTexInfo.Flags = ETextureFlags::Allow_DepthStencil;
		GraphicsHelper::InitRenderTextures(mainTexInfo, "Foward_TargetDepthTexture", &mTargetDepthTextures);
	}

	void FowardRenderer::InitProcesses()
	{
		AddPreProcess<PreRenderProcess_ComputeCluster>();
		AddPreProcess<PreRenderProcess_LightCulling>();

		AddPostProcess<PostProcess_Bloom>();
		AddPostProcess<PostProcess_ToneMapping>();
	}

	void FowardRenderer::InitRayTracing()
	{


	}

	void FowardRenderer::InitGlobalRenderParams()
	{
		mExposure = RP_Global_Float::Create("Renderer/Exposure", 2.0f, GetRenderParamManager());
		mInitialMinLog = RP_Global_Float::Create("Renderer/InitialMinLog", -12.0f, GetRenderParamManager());
		mInitialMaxLog = RP_Global_Float::Create("Renderer/InitialMaxLog", 4.0f, GetRenderParamManager());
	}


}