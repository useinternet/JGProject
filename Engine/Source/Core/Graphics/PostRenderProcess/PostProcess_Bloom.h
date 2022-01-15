#pragma once
#include "Graphics/RenderProcess.h"

namespace JG
{

	class IMaterial;
	class PostProcess_Bloom : public IRenderProcess
	{
		//// 그릴 텍스쳐
		//JVector2 mBloomResolutoin;
		//List<SharedPtr<ITexture>>  mBloomTextures[5][2];

		//List<SharedPtr<ITexture>>  mLumaTextures;
		//// Dispatch 할 Computer
		//List<SharedPtr<IComputer>> mExtractBrightnessComputers;
		//List<SharedPtr<IComputer>> mBloomDownSampleComputers;
		//List<SharedPtr<IComputer>> mBlurComputers;


		//JVector2 mPrevResolution;

		//RP_Global_Float mBloomThreshold;    // 4.0f, 0.0f, 8.0f
		//RP_Global_Float mUpSamplingFactor;  // 0.65, 0.0f, 1.0f
		//RP_Global_Float mBloomStrength;     // 0.1f, 0.0f, 2.0f, 0.05f);


		//RP_Global_Tex  mLumaResult;
		//RP_Global_Tex  mBloomResult;
	public:
		PostProcess_Bloom();
	public:
		virtual void Awake(const AwakeData& data) override;
		virtual void Ready(const ReadyData& data) override;
		virtual void Run(const RunData& data) override;
		virtual bool IsCompelete() override;
		virtual Type GetType() const override;
	private:
		//bool Blur(SharedPtr<IComputer> targetComputer, List<SharedPtr<ITexture>> texes, SharedPtr<ITexture> lowerTex, f32 upSamplingFactor);
		//bool InitComputers();
		//bool InitTextures(const JVector2& resolution);
	};
}