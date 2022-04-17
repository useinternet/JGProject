#include "pch.h"
#include"DeferredRenderer.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/RootSignature.h"
#include "Graphics/GraphicsHelper.h"
#include "Graphics/Manager/LightManager.h"
#include "Graphics/RayTracing/RayTracingPipeline.h"
#include "Graphics/RayTracing/TopLevelAccelerationStructure.h"
#include "Graphics/RayTracing/BottomLevelAccelerationStructure.h"
#include "Graphics/PreRenderProcess/PreRenderProcess_ComputeCluster.h"
#include "Graphics/PreRenderProcess/PreRenderProcess_LightCulling.h"

#include "Graphics/PostRenderProcess/PostProcess_Bloom.h"
#include "Graphics/PostRenderProcess/PostProcess_ToneMapping.h"
#include "Graphics/Batch/Render2DBatch.h"
#include "Graphics/RayTracing/Raytracer.h"
namespace JG
{
	DeferredRenderer::DeferredRenderer()
	{
		InitProcesses();
		InitGlobalRenderParams();
	}
	void DeferredRenderer::ReadyImpl(Graphics::RenderPassData* renderPassData)
	{
		const RenderInfo& info = GetRenderInfo();
		SharedPtr<IGraphicsContext> context = GetGraphicsContext();
		context->BindRootSignature(GetGraphicsRootSignature());


		if (mResolution != info.Resolution ||
			mClearColor != info.ClearColor)
		{
			mResolution = info.Resolution;
			mClearColor = info.ClearColor;
			InitTextures(info.Resolution, info.ClearColor);
		}
	}
	void DeferredRenderer::RenderImpl(SharedPtr<RenderResult> result)
	{
		UpdateGBufferPass();
		UpdateLightPass();
		UpdateFinalPass();
		result->SceneTexture = mFinalResultTex.GetValue();
	}
	void DeferredRenderer::CompeleteImpl(SharedPtr<RenderResult> result)
	{
	}
	int DeferredRenderer::ArrangeObject(const ObjectInfo& info)
	{
		return 0;
	}
	void DeferredRenderer::InitTextures(const JVector2& size, const Color& clearColor)
	{
		TextureInfo mainTexInfo;
		mainTexInfo.Width = std::max<u32>(1, size.x);
		mainTexInfo.Height = std::max<u32>(1, size.y);
		mainTexInfo.ArraySize = 1;
		mainTexInfo.Format = ETextureFormat::R32G32B32A32_Float;
		mainTexInfo.Flags = ETextureFlags::Allow_RenderTarget | ETextureFlags::Allow_UnorderedAccessView;;
		mainTexInfo.MipLevel = 1;
		mainTexInfo.ClearColor = clearColor;


		GraphicsHelper::InitRenderTextures(mainTexInfo, "Foward_TargetTexture", &mTargetTextures);

		mainTexInfo.Format = ETextureFormat::D24_Unorm_S8_Uint;
		mainTexInfo.Flags = ETextureFlags::Allow_DepthStencil;
		GraphicsHelper::InitRenderTextures(mainTexInfo, "Foward_TargetDepthTexture", &mTargetDepthTextures);





		mainTexInfo.Format = ETextureFormat::R8G8B8A8_Unorm;
		mainTexInfo.Flags = ETextureFlags::Allow_RenderTarget;
		mainTexInfo.ClearColor = Color();
		GraphicsHelper::InitRenderTextures(mainTexInfo, "Defferred_Albedo", &mGBufferDic[EGBuffer::Albedo]);
		GraphicsHelper::InitRenderTextures(mainTexInfo, "Defferred_Normal", &mGBufferDic[EGBuffer::Normal]);
		GraphicsHelper::InitRenderTextures(mainTexInfo, "Defferred_Specular", &mGBufferDic[EGBuffer::Specular]);
		mainTexInfo.Format = ETextureFormat::R11G11B10_Float;
		GraphicsHelper::InitRenderTextures(mainTexInfo, "Defferred_Emissive", &mGBufferDic[EGBuffer::Emissive]);


		mainTexInfo.Format = ETextureFormat::R8G8B8A8_Unorm;
		GraphicsHelper::InitRenderTextures(mainTexInfo, "Defferred_Material", &mGBufferDic[EGBuffer::Material_0]);

		mainTexInfo.Format = ETextureFormat::R16G16B16A16_Float;
		GraphicsHelper::InitRenderTextures(mainTexInfo, "Defferred_LightResult", &mLightResult);

		mainTexInfo.Format = ETextureFormat::R16G16B16A16_Float;
		GraphicsHelper::InitRenderTextures(mainTexInfo, "Defferred_Depth", &mGBufferDic[EGBuffer::WorldPos]);

		mainTexInfo.Format = ETextureFormat::R32_Float;
		mainTexInfo.ClearColor = Color::White();
		GraphicsHelper::InitRenderTextures(mainTexInfo, "Defferred_Depth", &mGBufferDic[EGBuffer::Depth]);

		mLightShader = ShaderLibrary::GetInstance().FindGraphicsShader(ShaderDefine::Template::StandardSceneShader, { "Scene/LightPass" });
		mFinalShader = ShaderLibrary::GetInstance().FindGraphicsShader(ShaderDefine::Template::StandardSceneShader, { "Scene/FinalPass" });
	}
	void DeferredRenderer::InitProcesses()
	{
		AddPreProcess<PreRenderProcess_ComputeCluster>();
		AddPreProcess<PreRenderProcess_LightCulling>();

		AddPostProcess<PostProcess_Bloom>();
		AddPostProcess<PostProcess_ToneMapping>();
	}
	void DeferredRenderer::InitGlobalRenderParams()
	{
		mGBufferTexDic[EGBuffer::Albedo] = RP_Global_Tex::Create("Renderer/GBuffer/Albedo", nullptr, GetRenderParamManager(), false);
		mGBufferTexDic[EGBuffer::Normal] = RP_Global_Tex::Create("Renderer/GBuffer/Normal", nullptr, GetRenderParamManager(), false);
		mGBufferTexDic[EGBuffer::Specular] = RP_Global_Tex::Create("Renderer/GBuffer/Specular", nullptr, GetRenderParamManager(), false);
		mGBufferTexDic[EGBuffer::Emissive] = RP_Global_Tex::Create("Renderer/GBuffer/Emissive", nullptr, GetRenderParamManager(), false);
		mGBufferTexDic[EGBuffer::Material_0] = RP_Global_Tex::Create("Renderer/GBuffer/Material_0", nullptr, GetRenderParamManager(), false);
		mGBufferTexDic[EGBuffer::Depth] = RP_Global_Tex::Create("Renderer/GBuffer/Depth", nullptr, GetRenderParamManager(), false);
		mGBufferTexDic[EGBuffer::WorldPos] = RP_Global_Tex::Create("Renderer/GBuffer/WorldPos", nullptr, GetRenderParamManager(), false);


		mLightResultTex = RP_Global_Tex::Create("Renderer/LightResult", nullptr, GetRenderParamManager(), false);
		mFinalResultTex = RP_Global_Tex::Create("Renderer/FinalResult", nullptr, GetRenderParamManager(), false);
	}
	void DeferredRenderer::UpdateGBufferPass()
	{
		u64 buffIndex = JGGraphics::GetInstance().GetBufferIndex();
		List<SharedPtr<ITexture>> targetTextures = {
			GetTargetTexture(EGBuffer::Albedo),
			GetTargetTexture(EGBuffer::Normal),
			GetTargetTexture(EGBuffer::WorldPos),
			GetTargetTexture(EGBuffer::Specular),
			GetTargetTexture(EGBuffer::Emissive),
			GetTargetTexture(EGBuffer::Material_0),
			GetTargetTexture(EGBuffer::Depth)
		};
		SharedPtr<ITexture> targetDepthTexture = mTargetDepthTextures[buffIndex];
		SharedPtr<IGraphicsContext> context = GetGraphicsContext();
		context->SetViewports({ Viewport(mResolution.x, mResolution.y) });
		context->SetScissorRects({ ScissorRect(0,0, mResolution.x,mResolution.y) });
		context->ClearRenderTarget(targetTextures, targetDepthTexture);
		context->SetRenderTarget(targetTextures, targetDepthTexture);

		ForEach([&](int objectType, const List<ObjectInfo>& objectList)
		{
			for (auto& info : objectList)
			{
				auto mesh = info.Mesh;
				auto& materialList = info.MaterialList;
				auto& worldMatrix = JMatrix::Transpose(info.WorldMatrix);

				context->BindConstantBuffer((u32)ERootParam::ObjectCB, worldMatrix);
				context->DrawIndexedAfterBindMeshAndMaterial(mesh, materialList);
			}
		});

		mGBufferTexDic[EGBuffer::Albedo].SetValue(targetTextures[0]);
		mGBufferTexDic[EGBuffer::Normal].SetValue(targetTextures[1]);
		mGBufferTexDic[EGBuffer::WorldPos].SetValue(targetTextures[2]);
		mGBufferTexDic[EGBuffer::Specular].SetValue(targetTextures[2]);
		mGBufferTexDic[EGBuffer::Emissive].SetValue(targetTextures[3]);
		mGBufferTexDic[EGBuffer::Material_0].SetValue(targetTextures[4]);
		mGBufferTexDic[EGBuffer::Depth].SetValue(targetTextures[5]);
	}
	void DeferredRenderer::UpdateLightPass()
	{
		u64 buffIndex = JGGraphics::GetInstance().GetBufferIndex();
		List<SharedPtr<ITexture>> bindTextures = {
			GetTargetTexture(EGBuffer::Albedo),
			GetTargetTexture(EGBuffer::Normal),
			GetTargetTexture(EGBuffer::WorldPos),
			GetTargetTexture(EGBuffer::Specular),
			GetTargetTexture(EGBuffer::Emissive),
			GetTargetTexture(EGBuffer::Material_0),
			GetTargetTexture(EGBuffer::Depth),
		};
		SharedPtr<IGraphicsContext> context = GetGraphicsContext();
		SharedPtr<ITexture> targetTex = mLightResult[buffIndex];



		context->SetViewports({ Viewport(mResolution.x, mResolution.y) });
		context->SetScissorRects({ ScissorRect(0,0, mResolution.x,mResolution.y) });
		context->ClearRenderTarget({ targetTex }, nullptr);
		context->SetRenderTarget({ targetTex }, nullptr);
		context->SetDepthStencilState(EDepthStencilStateTemplate::NoDepth);

		context->BindShader(mLightShader);
		context->BindTextures((u32)ERootParam::Texture2D, bindTextures);
		context->Draw(6);


		mLightResultTex.SetValue(targetTex);
	}
	void DeferredRenderer::UpdateFinalPass()
	{
		u64 buffIndex = JGGraphics::GetInstance().GetBufferIndex();
		List<SharedPtr<ITexture>> bindTextures = {
			RP_Global_Tex::Load("Renderer/LightResult", GetRenderParamManager()).GetValue(),
			GetTargetTexture(EGBuffer::Depth),
		};
		SharedPtr<IGraphicsContext> context = GetGraphicsContext();
		SharedPtr<ITexture> targetTex = mTargetTextures[buffIndex];

		context->SetViewports({ Viewport(mResolution.x, mResolution.y) });
		context->SetScissorRects({ ScissorRect(0,0, mResolution.x,mResolution.y) });
		context->ClearRenderTarget({ targetTex }, nullptr);
		context->SetRenderTarget({ targetTex }, nullptr);
		context->SetDepthStencilState(EDepthStencilStateTemplate::NoDepth);

		context->BindShader(mFinalShader);
		context->BindTextures((u32)ERootParam::Texture2D, bindTextures);
		context->Draw(6);


		mFinalResultTex.SetValue(targetTex);
	}
	SharedPtr<ITexture> DeferredRenderer::GetTargetTexture(EGBuffer buffer)
	{
		u64 buffIndex = JGGraphics::GetInstance().GetBufferIndex();
		return mGBufferDic[buffer][buffIndex];
	}
}