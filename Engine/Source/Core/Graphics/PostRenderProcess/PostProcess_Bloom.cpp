#include "pch.h"
#include "PostProcess_Bloom.h"
#include "Graphics/GraphicsHelper.h"



namespace JG
{
	PostProcess_Bloom::PostProcess_Bloom()
	{


	}

	void PostProcess_Bloom::Awake(Renderer* renderer)
	{
		mBloomThreshold   = RP_Global_Float::Create("PostProcess/Bloom/BloomThreshold", 4.0f, renderer->GetRenderParamManager());
		mBloomStrength    = RP_Global_Float::Create("PostProcess/Bloom/BloomStrength", 0.1f, renderer->GetRenderParamManager());
		mUpSamplingFactor = RP_Global_Float::Create("PostProcess/Bloom/UpSamplingFactor", 0.65f, renderer->GetRenderParamManager());

		mLumaResult     = RP_Global_Tex::Create("PostProcess/Bloom/LumaResult", nullptr, renderer->GetRenderParamManager());
		mBloomResult    = RP_Global_Tex::Create("PostProcess/Bloom/BloomResult", nullptr, renderer->GetRenderParamManager());
	}

	void PostProcess_Bloom::Ready(Renderer* renderer, IGraphicsAPI* api, Graphics::RenderPassData* rednerPassData, const RenderInfo& info)
	{
		InitComputers();
		if (mLumaTextures.empty() || mPrevResolution != info.Resolution)
		{
			InitTextures(info.Resolution);
			mPrevResolution = info.Resolution;
		}
	}

	void PostProcess_Bloom::Run(Renderer* renderer, IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result)
	{
		// Extract Brightness
		{
			SharedPtr<IComputer>& targetComputer    = mExtractBrightnessComputers[info.CurrentBufferIndex];
			SharedPtr<IStructuredBuffer> exposureSB = RP_Global_SB::Load("Renderer/Exposure", renderer->GetRenderParamManager()).GetValue();

			if (targetComputer->SetStructuredBuffer("Exposure", exposureSB) == false)
			{
				return;
			}

			if (targetComputer->SetTexture("SourceTex", 0, result->SceneTexture) == false)
			{
				return;
			}
			if (targetComputer->SetFloat("BloomThreshold", mBloomThreshold.GetValue()) == false)
			{
				return;
			}

			if (targetComputer->SetFloat2("InverseOutputSize", JVector2(1 / mBloomResolutoin.x, 1 / mBloomResolutoin.y)) == false)
			{
				return;
			}
			if (targetComputer->SetTexture("LumaResult", 0, mLumaTextures[info.CurrentBufferIndex]) == false)
			{
				return;
			}
			if (targetComputer->SetTexture("BloomResult", 0, mBloomTextures[0][0][info.CurrentBufferIndex]) == false)
			{
				return;
			}
			u32 groupX = Math::DivideByMultiple(mBloomResolutoin.x, 8);
			u32 groupY = Math::DivideByMultiple(mBloomResolutoin.y, 8);
			u32 groupZ = 1;

			targetComputer->Dispatch(groupX, groupY, groupZ, false);
		}

		// Bloom Downsample
		{
			SharedPtr<IComputer>& targetComputer = mBloomDownSampleComputers[info.CurrentBufferIndex];

			u32 groupX = Math::DivideByMultiple(mBloomResolutoin.x * 0.5f, 8);
			u32 groupY = Math::DivideByMultiple(mBloomResolutoin.y * 0.5f, 8);
			u32 groupZ = 1;

			targetComputer->Dispatch(groupX, groupY, groupZ, false);
		}



		{
	
			f32 upSamplingFactor = mUpSamplingFactor.GetValue();

			SharedPtr<IComputer> targetComputer = mBlurComputers[info.CurrentBufferIndex];
			List<SharedPtr<ITexture>> texes; texes.resize(2);
			SharedPtr<ITexture>       lowerTex = nullptr;

			//// 4 - 4,0
			texes[0] = mBloomTextures[4][0][info.CurrentBufferIndex];
			texes[1] = mBloomTextures[4][1][info.CurrentBufferIndex];
			lowerTex = mBloomTextures[4][0][info.CurrentBufferIndex];
			Blur(targetComputer, texes, lowerTex, 1.0f);


			// 3 - 4,1
			texes[0] = mBloomTextures[3][0][info.CurrentBufferIndex];
			texes[1] = mBloomTextures[3][1][info.CurrentBufferIndex];
			lowerTex = mBloomTextures[4][1][info.CurrentBufferIndex];
			Blur(targetComputer, texes, lowerTex, upSamplingFactor);


			// 2 - 3,1
			texes[0] = mBloomTextures[2][0][info.CurrentBufferIndex];
			texes[1] = mBloomTextures[2][1][info.CurrentBufferIndex];
			lowerTex = mBloomTextures[3][1][info.CurrentBufferIndex];
			Blur(targetComputer, texes, lowerTex, upSamplingFactor);


			// 1 - 2,1
			texes[0] = mBloomTextures[1][0][info.CurrentBufferIndex];
			texes[1] = mBloomTextures[1][1][info.CurrentBufferIndex];
			lowerTex = mBloomTextures[2][1][info.CurrentBufferIndex];
			Blur(targetComputer, texes, lowerTex, upSamplingFactor);


			// 0 - 1,1
			texes[0] = mBloomTextures[0][0][info.CurrentBufferIndex];
			texes[1] = mBloomTextures[0][1][info.CurrentBufferIndex];
			lowerTex = mBloomTextures[1][1][info.CurrentBufferIndex];
			Blur(targetComputer, texes, lowerTex, upSamplingFactor);
		}

		mLumaResult.SetValue(mLumaTextures[info.CurrentBufferIndex]);
		mBloomResult.SetValue(mBloomTextures[0][1][info.CurrentBufferIndex]);
	}

	bool PostProcess_Bloom::IsCompelete()
	{
		return true;
	}

	Type PostProcess_Bloom::GetType() const
	{
		return JGTYPE(PostProcess_Bloom);
	}

	bool PostProcess_Bloom::Blur(SharedPtr<IComputer> targetComputer, List<SharedPtr<ITexture>> texes, SharedPtr<ITexture> lowerTex, f32 upSamplingFactor)
	{
		TextureInfo texInfo = texes[0]->GetTextureInfo();

		if (targetComputer->SetFloat2("InverseDimensions", JVector2(1.0f / (f32)texInfo.Width, 1.0f / (f32)texInfo.Height)) == false)
		{
			return false;
		}
		if (targetComputer->SetFloat("UpsampleBlendFactor", upSamplingFactor) == false)
		{
			return false;
		}
		if (targetComputer->SetTexture("HigherResBuf", 0, texes[0]) == false)
		{
			return false;
		}

		if (targetComputer->SetTexture("LowerResBuf", 0, lowerTex) == false)
		{
			return false;
		}
		if (targetComputer->SetTexture("Result", 0, texes[1]) == false)
		{
			return false;
		}
		u32 groupX = Math::DivideByMultiple(texInfo.Width, 8);
		u32 groupY = Math::DivideByMultiple(texInfo.Height, 8);
		u32 groupZ = 1;

		targetComputer->Dispatch(groupX, groupY, 1, false);


		return true;
	}

	bool PostProcess_Bloom::InitComputers()
	{

		if (mExtractBrightnessComputers.empty())
		{
			SharedPtr<IComputeShader> shader = ShaderLibrary::GetInstance().FindComputeShader("Bloom_ExtractBrightness");
			if (shader == nullptr)
			{
				return false;
			}
			GraphicsHelper::InitComputer("ExtractBrightness_Computer", shader, &mExtractBrightnessComputers);
		}

		if (mBloomDownSampleComputers.empty())
		{
			auto shader = ShaderLibrary::GetInstance().FindComputeShader("Bloom_DwonSample");
			if (shader == nullptr)
			{
				return false;
			}
			GraphicsHelper::InitComputer("Bloom_Downsample_Computer", shader, &mBloomDownSampleComputers);
		}

		if (mBlurComputers.empty())
		{
			auto shader = ShaderLibrary::GetInstance().FindComputeShader("UpSampleAndBlur");
			if (shader == nullptr)
			{
				return false;
			}
			GraphicsHelper::InitComputer("UpSampleAndBlur_Computer", shader, &mBlurComputers);


		}
	
		return true;
	}

	bool PostProcess_Bloom::InitTextures(const JVector2& resolution)
	{
		mBloomResolutoin = resolution * 0.5f;

		TextureInfo texInfo;
		texInfo.ArraySize = 1;
		texInfo.Format    = ETextureFormat::R11G11B10_Float;
		texInfo.Flags     = ETextureFlags::Allow_RenderTarget | ETextureFlags::Allow_UnorderedAccessView;
		texInfo.MipLevel  = 1;

		// Bloom Texture »ý¼º
		f32 k = 1.0f;

		for (i32 i = 0; i < 5; ++i)
		{
			texInfo.Width  = mBloomResolutoin.x * k;
			texInfo.Height = mBloomResolutoin.y * k;

			for (i32 j = 0; j < 2; ++j)
			{
				GraphicsHelper::InitRenderTextures(texInfo, "Bloom_Texture_" + std::to_string(i) + std::to_string(j), &mBloomTextures[i][j]);
			}


			k *= 0.5f;
		}

		texInfo.Width  = mBloomResolutoin.x;
		texInfo.Height = mBloomResolutoin.y;
		texInfo.Format = ETextureFormat::R8_Uint;
		GraphicsHelper::InitRenderTextures(texInfo, "Luma_Textures", &mLumaTextures);




		// Computer 
		i32 index = 0;
		for (auto& computer : mExtractBrightnessComputers)
		{
			if (computer->SetFloat2("InverseOutputSize", JVector2(1 / mBloomResolutoin.x, 1 / mBloomResolutoin.y)) == false)
			{
				return false;
			}
			if (computer->SetTexture("LumaResult", 0, mLumaTextures[index]) == false)
			{
				return false;
			}
			if (computer->SetTexture("BloomResult", 0, mBloomTextures[0][0][index]) == false)
			{
				return false;
			}

			++index;
		}

		index = 0;
		for (auto& computer : mBloomDownSampleComputers)
		{
			if (computer->SetFloat2("InverseDimensions", JVector2(1 / mBloomResolutoin.x, 1 / mBloomResolutoin.y)) == false)
			{
				return false;
			}
			if (computer->SetTexture("BloomBuf", 0, mBloomTextures[0][0][index]) == false)
			{
				return false;
			}

			if (computer->SetTexture("Result1", 0, mBloomTextures[1][0][index]) == false)
			{
				return false;
			}
			if (computer->SetTexture("Result2", 0, mBloomTextures[2][0][index]) == false)
			{
				return false;
			}
			if (computer->SetTexture("Result3", 0, mBloomTextures[3][0][index]) == false)
			{
				return false;
			}
			if (computer->SetTexture("Result4", 0, mBloomTextures[4][0][index]) == false)
			{
				return false;
			}


			++index;
		}



		return true;
	}
}

