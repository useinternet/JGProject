#include "pch.h"
#include "PostProcess_ToneMapping.h"
#include "Graphics/GraphicsHelper.h"
#include "Graphics/PostRenderProcess/PostProcess_Bloom.h"


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
		if (mToneMappingComputers.empty())
		{
			if (InitComputers() == false)
			{

			}
		}
		if (mPrevResolution != info.Resolution)
		{
			mPrevResolution = info.Resolution;
			u64 bufferCnt = JGGraphics::GetInstance().GetBufferCount();
		

			mToneMappingTextures.resize(bufferCnt);
			if (InitTextures(info.Resolution) == false)
			{

			}
		}
	}

	void PostProcess_ToneMapping::Run(Renderer* renderer, IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result)
	{
		auto commandID = JGGraphics::GetInstance().RequestCommandID();


		SharedPtr<IComputer> targetComputer = mToneMappingComputers[info.CurrentBufferIndex];
		SharedPtr<ITexture>  targetTexture   = mToneMappingTextures[info.CurrentBufferIndex];
		SharedPtr<IStructuredBuffer> exposureSB = RP_Global_SB::Load("Renderer/Exposure", renderer->GetRenderParamManager()).GetValue();


		f32 bloomStrength			 = RP_Global_Float::Load("PostProcess/Bloom/BloomStrength", renderer->GetRenderParamManager()).GetValue();
		SharedPtr<ITexture> bloomTex = RP_Global_Tex::Load("PostProcess/Bloom/BloomResult", renderer->GetRenderParamManager()).GetValue();


		if (targetComputer->SetStructuredBuffer("Exposure", exposureSB) == false)
		{
			return;
		}
		if (targetComputer->SetTexture("Bloom", 0, bloomTex) == false)
		{
			return;
		}
		if (targetComputer->SetTexture("SrcColor", 0, result->SceneTexture) == false)
		{
			return;
		}
		if (targetComputer->SetFloat("BloomStrength", bloomStrength) == false)
		{
			return;
		}
		u32 groupX = Math::DivideByMultiple(info.Resolution.x, 8);
		u32 groupY = Math::DivideByMultiple(info.Resolution.y, 8);

		targetComputer->Dispatch(commandID, groupX, groupY, 1, false);
		result->SceneTexture = mToneMappingTextures[info.CurrentBufferIndex];
	}

	bool PostProcess_ToneMapping::IsCompelete()
	{
		return true;
	}

	Type PostProcess_ToneMapping::GetType() const
	{
		return JGTYPE(PostProcess_ToneMapping);
	}
	bool PostProcess_ToneMapping::InitComputers()
	{
		SharedPtr<IComputeShader> shader = ShaderLibrary::GetInstance().FindComputeShader("Tonemapping");
		if (shader == nullptr)
		{
			return false;
		}
		GraphicsHelper::InitComputer("ToneMapping_Computer", shader, &mToneMappingComputers);
		return true;
	}
	bool PostProcess_ToneMapping::InitTextures(const JVector2& size)
	{
		TextureInfo mainTexInfo;
		mainTexInfo.Width = std::max<u32>(1, size.x);
		mainTexInfo.Height = std::max<u32>(1, size.y);
		mainTexInfo.ArraySize = 1;
		mainTexInfo.Format = ETextureFormat::R16G16B16A16_Float;
		mainTexInfo.Flags = ETextureFlags::Allow_UnorderedAccessView | ETextureFlags::Allow_RenderTarget;
		mainTexInfo.MipLevel = 1;


		GraphicsHelper::InitRenderTextures(mainTexInfo, "ToneMapping_TargetTexture", &mToneMappingTextures);


		mainTexInfo.Format = ETextureFormat::R8_Unorm;
		GraphicsHelper::InitRenderTextures(mainTexInfo, "OutLuma_Texture", &mOutLumaTextures);




		i32 index = 0;
		for (auto& computer : mToneMappingComputers)
		{
			if (computer->SetFloat2("RcpBufferDim", JVector2(1 / size.x, 1 / size.y)) == false)
			{
				return false;
			}
			if (computer->SetTexture("DestColor", 0, mToneMappingTextures[index]) == false)
			{
				return false;
			}
			if (computer->SetTexture("OutLuma", 0, mOutLumaTextures[index]) == false)
			{
				return false;
			}

			++index;
		}
		return true;
	}
}

