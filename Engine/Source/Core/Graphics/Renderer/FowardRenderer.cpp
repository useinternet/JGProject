#include "pch.h"
#include "FowardRenderer.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/GraphicsHelper.h"
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
	}

	void FowardRenderer::ReadyImpl(IGraphicsAPI* api, Graphics::RenderPassData* renderPassData, const RenderInfo& info)
	{
		auto commandID = JGGraphics::GetInstance().RequestCommandID();


		if (mPrevResolution != info.Resolution ||
			mPrevClearColor != info.ClearColor ||
			mPrevIsHDR != info.IsHDR)
		{

			mPrevIsHDR = info.IsHDR;
			mPrevResolution = info.Resolution;
			mPrevClearColor = info.ClearColor;
			InitTextures(info.Resolution, info.ClearColor, info.IsHDR);
		}
	}

	void FowardRenderer::RenderImpl(IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result)
	{
		auto commandID = JGGraphics::GetInstance().RequestCommandID();


		auto targetTexture = mTargetTextures[info.CurrentBufferIndex];
		auto targetDepthTexture = mTargetDepthTextures[info.CurrentBufferIndex];

		if (targetTexture == nullptr || targetDepthTexture == nullptr)
		{
			return;
		}


		api->SetViewports(commandID, { Viewport(info.Resolution.x, info.Resolution.y) });
		api->SetScissorRects(commandID, { ScissorRect(0,0, info.Resolution.x,info.Resolution.y) });
		api->ClearRenderTarget(commandID, { targetTexture }, targetDepthTexture);
		api->SetRenderTarget(commandID, { targetTexture }, targetDepthTexture);
		
		ForEach([&](int objectType, const List<ObjectInfo>& objectList)
		{
			for (auto& info : objectList)
			{
				auto mesh = info.Mesh;
				auto& materialList = info.MaterialList;
				auto& worldMatrix  = JMatrix::Transpose(info.WorldMatrix);
				api->SetTransform(commandID, &worldMatrix);

				if (mesh->Bind(commandID) == false)
				{
					JG_CORE_ERROR("{0} : Fail Mesh Bind", mesh->GetName());
				}
				for (u64 i = 0; i < mesh->GetSubMeshCount(); ++i)
				{
					if (mesh->GetSubMesh(i)->Bind(commandID) == false)
					{
						JG_CORE_ERROR("{0} : Fail Mesh Bind", mesh->GetSubMesh(i)->GetName());
						continue;
					}
					SharedPtr<IMaterial> material = nullptr;
					if (materialList.size() <= i)
					{
						material = materialList[0];
					}
					else
					{
						material = materialList[i];
					}

					if (material == nullptr || material->Bind(commandID) == false)
					{
						continue;
					}
					api->DrawIndexed(commandID, mesh->GetSubMesh(i)->GetIndexCount(), mesh->GetSubMesh(i)->GetInstanceCount());
				}
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

	void FowardRenderer::InitTextures(const JVector2& size, const Color& clearColor, bool ishdr)
	{
		TextureInfo mainTexInfo;
		mainTexInfo.Width  = std::max<u32>(1, size.x);
		mainTexInfo.Height = std::max<u32>(1, size.y);
		mainTexInfo.ArraySize = 1;
		mainTexInfo.Format = (ishdr) ? ETextureFormat::R16G16B16A16_Float : ETextureFormat::R8G8B8A8_Unorm;
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

	void FowardRenderer::InitGlobalRenderParams()
	{


//    Exposure, 1.0f / Exposure, Exposure, 0.0f,
//    kInitialMinLog, kInitialMaxLog, kInitialMaxLog - kInitialMinLog, 1.0f / (kInitialMaxLog - kInitialMinLog)


		// 2.0f, -8.0f, 8.0f, 0.25f
		RP_Global_Float::Create("Renderer/Exposure", 2.0f, this);
		RP_Global_Float::Create("Renderer/InitialMinLog", -12.0f, this);
		RP_Global_Float::Create("Renderer/InitialMaxLog", 4.0f, this);
		RP_Global_Float::Create("Renderer/HDRPaperWhite", 3.0f, this);
		RP_Global_Float::Create("Renderer/MaxDisplayLuminance", 500.0f, this);

		//200.0f, 100.0f, 500.0f, 50.0f
// MaxDisplayLuminance = 1000.0f, 500.0f, 10000.0f, 100.0f
		// RenderParam
		// Bloom ����

		// ToneMapHDRCS

		// UpdateExposure


	}


}