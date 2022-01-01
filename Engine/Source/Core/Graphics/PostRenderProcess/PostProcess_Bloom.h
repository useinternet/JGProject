#pragma once
#include "Graphics/RenderProcess.h"

namespace JG
{

	class IMaterial;
	class PostProcess_Bloom : public IRenderProcess
	{
		// 그릴 텍스쳐
		JVector2 mBloomResolutoin;
		//
		//[Down/Up SamplingIndex][Dest/Src][bufferIndex]
		List<SharedPtr<ITexture>>  mBloomTextures[5][2];

		List<SharedPtr<ITexture>>  mLumaTextures;
		// Dispatch 할 Computer
		List<SharedPtr<IComputer>> mExtractBrightnessComputers;
		List<SharedPtr<IComputer>> mBloomDownSampleComputers;



		JVector2 mPrevResolution;

		RP_Local_Bool  mEnable;
		RP_Local_Float mBloomThreshold;    // 4.0f, 0.0f, 8.0f
		RP_Local_Float mUpSamplingFactor;  // 0.65, 0.0f, 1.0f



		RP_Global_Tex  mLumaResult;
		RP_Global_Tex  mBloomResult;




		// 1 2 3 4 5

		// 1 -> brightness
		// 2 3 4 5 -> downsample
		// 5 -> 5 upsample
		// 
	public:
		PostProcess_Bloom();
	public:
		virtual void Awake(Renderer* renderer) override;
		virtual void Ready(Renderer* renderer, IGraphicsAPI* api, Graphics::RenderPassData* rednerPassData, const RenderInfo& info) override;
		virtual void Run(Renderer* renderer, IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result) override;
		virtual bool IsCompelete() override;
		virtual Type GetType() const override;
	private:
		bool InitComputers();
		bool InitTextures(const JVector2& resolution);
	};
}