#include "pch.h"
#include "PostProcess_ToneMapping.h"
#include "Graphics/GraphicsHelper.h"



namespace JG
{
	PostProcess_ToneMapping::PostProcess_ToneMapping()
	{



	}

	void PostProcess_ToneMapping::Awake(Renderer* renderer)
	{

	}

	void PostProcess_ToneMapping::Ready(Renderer* renderer, IGraphicsAPI* api, Graphics::RenderPassData* renderPassData, const RenderInfo& info)
	{
		if (mComputers.empty())
		{
			InitComputers();
		}
		if (mPrevResolution != info.Resolution)
		{
			mPrevResolution = info.Resolution;
			u64 bufferCnt = JGGraphics::GetInstance().GetBufferCount();
		

			mTargetTextures.resize(bufferCnt);
			InitTextures(info.Resolution);
		}
	}

	void PostProcess_ToneMapping::Run(Renderer* renderer, IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result)
	{
		if (mComputers.empty() || mTargetTextures.empty())
		{
			return;
		}
		auto commandID = JGGraphics::GetInstance().RequestCommandID();


		SharedPtr<IComputer> targetComputer = mComputers[info.CurrentBufferIndex];
		SharedPtr<ITexture>  targetTexture   = mTargetTextures[info.CurrentBufferIndex];


		if (targetComputer->SetFloat2("RcpBufferDim", JVector2(1.0f / info.Resolution.x, 1.0f / info.Resolution.y)) == false)
		{
			return;
		}
		if (targetComputer->SetTexture("SrcColor", 0, result->SceneTexture) == false)
		{
			return;
		}
		if (targetComputer->SetTexture("DestColor", 0, targetTexture) == false)
		{
			return;
		}

		if (targetComputer->Dispatch(commandID, info.Resolution.x / 8, info.Resolution.y / 8, 1, nullptr, false) == false)
		{
			return;
		}
		result->SceneTexture = mTargetTextures[info.CompeleteBufferIndex];
	}

	bool PostProcess_ToneMapping::IsCompelete()
	{
		return true;
	}

	Type PostProcess_ToneMapping::GetType() const
	{
		return JGTYPE(PostProcess_ToneMapping);
	}
	void PostProcess_ToneMapping::InitComputers()
	{
		SharedPtr<IComputeShader> shader = ShaderLibrary::GetInstance().FindComputeShader("Tonemapping");
		
		GraphicsHelper::InitComputer("ToneMapping_Computer", shader, &mComputers);

	}
	void PostProcess_ToneMapping::InitTextures(const JVector2& size)
	{
		TextureInfo mainTexInfo;
		mainTexInfo.Width = std::max<u32>(1, size.x);
		mainTexInfo.Height = std::max<u32>(1, size.y);
		mainTexInfo.ArraySize = 1;
		mainTexInfo.Format = ETextureFormat::R16G16B16A16_Float;
		mainTexInfo.Flags = ETextureFlags::Allow_UnorderedAccessView | ETextureFlags::Allow_RenderTarget;
		mainTexInfo.MipLevel = 1;


		GraphicsHelper::InitRenderTextures(mainTexInfo, "ToneMapping_TargetTexture", &mTargetTextures);
	}
}

