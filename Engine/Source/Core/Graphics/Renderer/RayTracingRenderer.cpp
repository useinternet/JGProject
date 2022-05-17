#include "pch.h"
#include "RayTracingRenderer.h"
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
	RayTracingRenderer::RayTracingRenderer()
	{
		InitProcesses();
		InitGlobalRenderParams();
		Init();
	}

	void RayTracingRenderer::ReadyImpl(Graphics::RenderPassData* renderPassData)
	{
		if (mRayTracer == nullptr)
		{
			return;
		}
		mRayTracer->Reset();

		const RenderInfo& info = GetRenderInfo();

		if (mResolution != info.Resolution ||
			mClearColor != info.ClearColor)
		{
			mResolution = info.Resolution;
			mClearColor = info.ClearColor;
			InitTextures(info.Resolution, info.ClearColor);
		}

		SharedPtr<IGraphicsContext> context = GetGraphicsContext();
		context->BindRootSignature(GetGraphicsRootSignature());
		context->ClearRenderTarget({ mTargetTexture }, nullptr);

	}

	void RayTracingRenderer::RenderImpl(SharedPtr<RenderResult> result)
	{
		if (mRayTracer == nullptr)
		{
			return;
		}
		ForEach([&](int objectType, const List<ObjectInfo>& objectList)
		{
			for (auto& info : objectList)
			{
				auto mesh = info.Mesh;
				auto materialList = info.MaterialList;
				auto& worldMatrix = JMatrix::Transpose(info.WorldMatrix);

				if ((info.Flags & Graphics::ESceneObjectFlags::Ignore_RayTracing_Bottom_Level_AS) == false)
				{
					u64 subMeshCount = mesh->GetSubMeshCount();
					for (u64 i = 0; i < subMeshCount; ++i)
					{
						SharedPtr<ISubMesh> subMesh = mesh->GetSubMesh(i);
						SharedPtr<IMaterial> material = nullptr;
						if (subMesh == nullptr)
						{
							continue;
						}
						if (i >= materialList.size())
						{
							material = materialList[0];
						}
						else
						{
							material = materialList[i];
						}
						mRayTracer->AddInstance(subMesh, material, { worldMatrix }, info.Flags);
					}
				}
			}
		});
		mRayTracer->Execute(GetComputeContext(), mTargetTexture);
		result->SceneTexture = mTargetTexture;
	}

	void RayTracingRenderer::CompeleteImpl(SharedPtr<RenderResult> result)
	{
	}

	int RayTracingRenderer::ArrangeObject(const ObjectInfo& info)
	{
		return 0;
	}

	void RayTracingRenderer::InitTextures(const JVector2& size, const Color& clearColor)
	{
		TextureInfo mainTexInfo;
		mainTexInfo.Width     = std::max<u32>(1, size.x);
		mainTexInfo.Height    = std::max<u32>(1, size.y);
		mainTexInfo.ArraySize = 1;
		mainTexInfo.Format    = ETextureFormat::R32G32B32A32_Float;
		mainTexInfo.Flags     = ETextureFlags::Allow_RenderTarget | ETextureFlags::Allow_UnorderedAccessView;
		mainTexInfo.MipLevel  = 1;
		mainTexInfo.ClearColor = clearColor;
		mTargetTexture = ITexture::Create("RayTracing_TargetTexture", mainTexInfo);
		mRayTracer->SetResolution(size);


	}

	void RayTracingRenderer::InitProcesses()
	{
		AddPreProcess<PreRenderProcess_ComputeCluster>();
		AddPreProcess<PreRenderProcess_LightCulling>();
	}

	void RayTracingRenderer::Init()
	{
		if (JGGraphics::GetInstance().IsSupportedRayTracing() == false)
		{
			return;
		}
		mRayTracer = CreateSharedPtr<RayTracer>(this);
	}

	void RayTracingRenderer::InitGlobalRenderParams()
	{
	}
}