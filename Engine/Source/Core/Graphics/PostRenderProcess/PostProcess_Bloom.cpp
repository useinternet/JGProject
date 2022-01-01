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
		mEnable         = RP_Local_Bool::Create("Enable", true, GetType(), renderer);
		mBloomThreshold = RP_Local_Float::Create("BloomThreshold", 4.0f, GetType(), renderer);
		mUpSamplingFactor = RP_Local_Float::Create("UpSamplingFactor", 0.65f, GetType(), renderer);

		mLumaResult     = RP_Global_Tex::Create("PostProcess/Bloom/LumaResult", nullptr, renderer);
		mBloomResult    = RP_Global_Tex::Create("PostProcess/Bloom/BloomResult", nullptr, renderer);

	}

	void PostProcess_Bloom::Ready(Renderer* renderer, IGraphicsAPI* api, Graphics::RenderPassData* rednerPassData, const RenderInfo& info)
	{
		if (mEnable.GetValue() == false)
		{
			return;
		}

		bool result = InitComputers();
		if (result == false)
		{
			mEnable.SetValue(false);
			return;
		}


		if (mLumaTextures.empty() || mPrevResolution != info.Resolution)
		{
			result = InitTextures(info.Resolution);
			mPrevResolution = info.Resolution;
		}
		if (result == false)
		{
			mEnable.SetValue(false);
			return;
		}

	}

	void PostProcess_Bloom::Run(Renderer* renderer, IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result)
	{
		if (mEnable.GetValue() == false)
		{
			return;
		}


		u64 commandID = JGGraphics::GetInstance().RequestCommandID();
		// Bloom
		{
			f32 exposureVal = RP_Global_Float::Load("Renderer/Exposure", renderer).GetValue();
			f32 initialMinLogVal = RP_Global_Float::Load("Renderer/InitialMinLog", renderer).GetValue();
			f32 initialMaxLogVal = RP_Global_Float::Load("Renderer/InitialMaxLog", renderer).GetValue();


			SharedPtr<IComputer>& targetComputer = mExtractBrightnessComputers[info.CurrentBufferIndex];


			List<f32> exposure; exposure.resize(8);

			exposure[0] = exposureVal;
			exposure[1] = 1.0f / exposureVal;
			exposure[2] = exposureVal;
			exposure[3] = 0.0f;
			exposure[4] = initialMinLogVal;
			exposure[5] = initialMaxLogVal;
			exposure[6] = initialMaxLogVal - initialMinLogVal;
			exposure[7] = 1.0f / (initialMaxLogVal - initialMinLogVal);



			if (targetComputer->SetStructDataArray("Exposure", exposure.data(), exposure.size(), sizeof(f32)) == false)
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
			u32 groupX = Math::DivideByMultiple(mBloomResolutoin.x, 8);
			u32 groupY = Math::DivideByMultiple(mBloomResolutoin.y, 8);
			u32 groupZ = 1;

			targetComputer->Dispatch(commandID, groupX, groupY, groupZ, nullptr, false);

		}
		//// Downsample Bloom
		//{
		//	SharedPtr<IComputer>& targetComputer = mDownSampleBloomComputers[info.CurrentBufferIndex];

		//	u32 groupX = Math::DivideByMultiple(mBloomResolutoin.x * 0.5f, 8);
		//	u32 groupY = Math::DivideByMultiple(mBloomResolutoin.y * 0.5f, 8);
		//	u32 groupZ = 1;
		//	targetComputer->Dispatch(commandID, groupX, groupY, groupZ, nullptr, false);
		//}


		//mLumaResult.SetValue(mLumaTextures[info.CompeleteBufferIndex]);
		//mBloomResult.SetValue(mBloomTextures[info.CompeleteBufferIndex]);


		result->SceneTexture   = mBloomTextures[0][0][info.CurrentBufferIndex];
		//result->SceneTexture = mBloomTextures[info.CurrentBufferIndex];
	}

	bool PostProcess_Bloom::IsCompelete()
	{
		return true;
	}

	Type PostProcess_Bloom::GetType() const
	{
		return JGTYPE(PostProcess_Bloom);
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


		return true;
	}

	bool PostProcess_Bloom::InitTextures(const JVector2& resolution)
	{
		mBloomResolutoin = resolution;

		TextureInfo texInfo;
		texInfo.ArraySize = 1;
		texInfo.Format    = ETextureFormat::R16G16B16A16_Float;
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


		return true;
	}
}

