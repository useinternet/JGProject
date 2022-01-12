#include "pch.h"
#include "FowardRenderer.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/GraphicsHelper.h"
#include "Graphics/PreRenderProcess/PreRenderProcess_ComputeCluster.h"
#include "Graphics/PreRenderProcess/PreRenderProcess_LightCulling.h"

#include "Graphics/PostRenderProcess/PostProcess_Bloom.h"
#include "Graphics/PostRenderProcess/PostProcess_ToneMapping.h"
#include "Graphics/PostRenderProcess/PostProcess_UpdateExposure.h"
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

		mExposureSB[info.CurrentBufferIndex]->SetData(sizeof(f32), 8, val.data(), commandID);
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

	void FowardRenderer::InitGlobalRenderParams()
	{
		mExposure = RP_Global_Float::Create("Renderer/Exposure", 2.0f, GetRenderParamManager());
		mInitialMinLog = RP_Global_Float::Create("Renderer/InitialMinLog", -12.0f, GetRenderParamManager());
		mInitialMaxLog = RP_Global_Float::Create("Renderer/InitialMaxLog", 4.0f, GetRenderParamManager());
	}


}