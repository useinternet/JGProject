#include "pch.h"
#include "FowardRenderer.h"
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

	FowardRenderer::FowardRenderer()
	{
		InitProcesses();
		InitGlobalRenderParams();
		InitRayTracing();
	}

	void FowardRenderer::ReadyImpl(Graphics::RenderPassData* renderPassData)
	{
		if (mRayTracer != nullptr)
		{
			mRayTracer->Reset();
		}
		const RenderInfo& info = GetRenderInfo();

		SharedPtr<IGraphicsContext> context = GetGraphicsContext();

		context->BindRootSignature(GetGraphicsRootSignature());



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

	void FowardRenderer::RenderImpl(SharedPtr<RenderResult> result)
	{

		UpdateRayTacing();

		const RenderInfo& info = GetRenderInfo();
		auto targetTexture = mTargetTextures[info.CurrentBufferIndex];
		auto targetDepthTexture = mTargetDepthTextures[info.CurrentBufferIndex];

		if (targetTexture == nullptr || targetDepthTexture == nullptr)
		{
			return;
		}
		SharedPtr<IGraphicsContext> context = GetGraphicsContext();
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

				context->BindConstantBuffer((u32)ERootParam::ObjectCB, worldMatrix);
				context->DrawIndexedAfterBindMeshAndMaterial(mesh, materialList);
			}
		});
		if (result != nullptr)
		{
			result->SceneTexture = mTargetTextures[info.CurrentBufferIndex];
		}
	}

	void FowardRenderer::CompeleteImpl(SharedPtr<RenderResult> result)
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

		if (mRayTracer != nullptr && JGGraphics::GetInstance().IsSupportedRayTracing())
		{
			mRayTracer->SetResolution(size);
		}
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
		if (JGGraphics::GetInstance().IsSupportedRayTracing() == false)
		{
			return;
		}
		mRayTracer = CreateSharedPtr<RayTracer>(this);
	}



	void FowardRenderer::InitGlobalRenderParams()
	{
		mExposure = RP_Global_Float::Create("Renderer/Exposure", 2.0f, GetRenderParamManager());
		mInitialMinLog = RP_Global_Float::Create("Renderer/InitialMinLog", -12.0f, GetRenderParamManager());
		mInitialMaxLog = RP_Global_Float::Create("Renderer/InitialMaxLog", 4.0f, GetRenderParamManager());
	}

	void FowardRenderer::UpdateBottomLevelAS(SharedPtr<IMesh> mesh, const JMatrix& worldMatrix)
	{
		if (JGGraphics::GetInstance().IsSupportedRayTracing() == false)
		{
			return;
		}
		if (mesh == nullptr)
		{
			return;
		}
		// RayTracing AS »ý¼º
		u32 subMeshCount = mesh->GetSubMeshCount();
		for (i32 i = 0; i < subMeshCount; ++i)
		{
			SharedPtr<ISubMesh> subMesh = mesh->GetSubMesh(i);

			if (subMesh == nullptr)
			{
				continue;
			}
			SharedPtr<IBottomLevelAccelerationStructure> blas = subMesh->GetBottomLevelAS();

			if (blas == nullptr)
			{
				blas = IBottomLevelAccelerationStructure::Create();
				blas->Generate(GetComputeContext(), subMesh->GetVertexBuffer(), subMesh->GetIndexBuffer());
				subMesh->SetBottomLevelAS(blas);
			}
			u64 instanceCount = subMesh->GetInstanceCount();
			for (u64 insID = 0; insID < instanceCount; ++insID)
			{
				mRayTracer->AddInstance(blas, worldMatrix, insID, 0);
			}

		}


	}
	void FowardRenderer::UpdateRayTacing()
	{
		if (JGGraphics::GetInstance().IsSupportedRayTracing() == false || mRayTracer ==nullptr)
		{
			return;
		}

		ForEach([&](int objectType, const List<ObjectInfo>& objectList)
		{
			for (auto& info : objectList)
			{
				auto mesh = info.Mesh;
				auto& worldMatrix = JMatrix::Transpose(info.WorldMatrix);

				if ((info.Flags & Graphics::ESceneObjectFlags::Ignore_RayTracing_Bottom_Level_AS) == false)
				{
					UpdateBottomLevelAS(mesh, worldMatrix);
				}
			}
		});
		mRayTracer->Execute(GetComputeContext());
	}
}