#include "pch.h"
#include "PostProcess_UpdateExposure.h"
#include "Graphics/GraphicsHelper.h"


namespace JG
{
	PostProcess_UpdateExposure::PostProcess_UpdateExposure()
	{


	}

	void PostProcess_UpdateExposure::Awake(Renderer* renderer)
	{
		InitComputers();
		InitTextures();
	}

	void PostProcess_UpdateExposure::Ready(Renderer* renderer, IGraphicsAPI* api, Graphics::RenderPassData* rednerPassData, const RenderInfo& info)
	{
		


	}

	void PostProcess_UpdateExposure::Run(Renderer* renderer, IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result)
	{
		u64 commandID = JGGraphics::GetInstance().RequestCommandID();

		RP_Global_Tex lumaResultVal  = RP_Global_Tex::Load("PostProcess/Bloom/LumaResult", renderer->GetRenderParamManager());
		RP_Global_SB  exposureSBVal = RP_Global_SB::Load("Renderer/Exposure", renderer->GetRenderParamManager());


		RP_Global_Float minExposureVal = RP_Global_Float::Load("Renderer/MinExposure", renderer->GetRenderParamManager());
		RP_Global_Float maxExposureVal = RP_Global_Float::Load("Renderer/MaxExposure", renderer->GetRenderParamManager());
		RP_Global_Float targetLuminanceVal = RP_Global_Float::Load("Renderer/TargetLuminance", renderer->GetRenderParamManager());
		RP_Global_Float adaptationRateVal = RP_Global_Float::Load("Renderer/AdaptationRate", renderer->GetRenderParamManager());


		TextureInfo texInfo = lumaResultVal.GetValue()->GetTextureInfo();


		{
			auto targetComputer = mGenerateHistrogramComputers[info.CurrentBufferIndex];
			auto targetHistogram = mHistograms[info.CurrentBufferIndex];
			targetComputer->SetUint("kBufferHeight", texInfo.Height);
			targetComputer->SetTexture("LumaBuf", 0, lumaResultVal.GetValue());

			u32 groupX = Math::DivideByMultiple(texInfo.Width, 16);
			u32 groupY = Math::DivideByMultiple(texInfo.Height, texInfo.Height);
			u32 groupZ = 1;

			api->ClearUAVUint(commandID, targetHistogram);
			targetComputer->Dispatch(commandID, groupX, groupY, groupZ, false);

		}
		{
			auto targetComputer = mAdaptExposureComputers[info.CurrentBufferIndex];
			
			targetComputer->SetFloat("TargetLuminance", targetLuminanceVal.GetValue());
			targetComputer->SetFloat("AdaptationRate", adaptationRateVal.GetValue());
			targetComputer->SetFloat("MinExposure", minExposureVal.GetValue());
			targetComputer->SetFloat("MaxExposure", maxExposureVal.GetValue());


			u32 pixelCount = texInfo.Width * texInfo.Height;
			targetComputer->SetUint("PixelCount", pixelCount);
			targetComputer->SetStructuredBuffer("Exposure", exposureSBVal.GetValue());

			targetComputer->Dispatch(commandID, 1, 1, 1, false);
		}


	}

	bool PostProcess_UpdateExposure::IsCompelete()
	{
		return true;
	}

	Type PostProcess_UpdateExposure::GetType() const
	{
		return JGTYPE(PostProcess_UpdateExposure);
	}

	bool PostProcess_UpdateExposure::InitComputers()
	{
		if (mGenerateHistrogramComputers.empty())
		{
			auto shader = ShaderLibrary::GetInstance().FindComputeShader("GenerateHistogram");
			if (shader == nullptr)
			{
				return false;
			}

			GraphicsHelper::InitComputer("GenerateHistogram", shader, &mGenerateHistrogramComputers);
		}
		if (mAdaptExposureComputers.empty())
		{
			auto shader = ShaderLibrary::GetInstance().FindComputeShader("AdaptExposure");
			if (shader == nullptr)
			{
				return false;
			}

			GraphicsHelper::InitComputer("AdaptExposure", shader, &mAdaptExposureComputers);

		}

		return true;
	}

	bool PostProcess_UpdateExposure::InitTextures()
	{
		if (mHistograms.empty())
		{
			GraphicsHelper::InitByteAddressBuffer("Histogram", 256, &mHistograms);
		}


		i32 index = 0;
		for (auto& c : mGenerateHistrogramComputers)
		{
			c->SetByteAddressBuffer("Histogram", mHistograms[index]);
		    index++;
		}
		index = 0;
		for (auto& c : mAdaptExposureComputers)
		{
			c->SetByteAddressBuffer("Histogram", mHistograms[index]);
			index++;
		}
		return true;
	}

}
