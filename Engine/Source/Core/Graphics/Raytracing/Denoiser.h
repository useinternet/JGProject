#pragma once
#include "JGCore.h"
#include "Graphics/RenderParam.h"

namespace JG
{
	enum class ETextureFormat;
	class IComputeContext;
	class Renderer;
	class ITexture;
	class TemporalSupersampling_ReverseReproject;
	class TemporalSupersampling_BlendWithCurrentFrame;
	class AtrousWaveletTransformCrossBilateralFilter;
	class CalculateMeanVariance;
	class DisocclusionBilateralFilter;
	class Denoiser
	{
		class ETemporalSuperSampling
		{
		public:
			enum
			{
				Tspp,
				RayHitDistance,
				CoefficientSquaredMean,
				Count,
			};
		};
	public:
		struct Input
		{
			JVector2 Resolution;
			SharedPtr<ITexture> NormalDepth;
			SharedPtr<ITexture> DepthDerivatives;
			SharedPtr<ITexture> ReprojectedNormalDepth;
			SharedPtr<ITexture> MotionVector;
			SharedPtr<ITexture> Value;
			SharedPtr<ITexture> RayHitDistance;
			SharedPtr<ITexture> Depth;

			f32 MaxRayDistance;
		};
		struct Output
		{
			SharedPtr<ITexture> OutValue;
		};
		
	private:
		Renderer*		mRenderer;
		String			mName;

		JVector2  mResolution;
		RP_Global_Float mDepthSigma;
		RP_Global_Int mVariance_BilateralFilterKernelWidth;
		RP_Global_Int mMaxTspp;
		RP_Global_Float mStdDevGamma;
		RP_Global_Float mClampingMinStdDevTolerance;
		RP_Global_Float mClampDifferenceToTsppScale;


		RP_Global_Int mMinTsppToUseTemporalVariance;
		RP_Global_Int mBlurStrengthMaxTspp;
		RP_Global_Float mBlurDecayStrength;


		RP_Global_Bool mKernelRadiusRotateKernelEnable; 
		RP_Global_Int  mKernelRadiusRotateKernelNumCycles; 
		RP_Global_Float mAdaptiveKernelSizeRayHitDistanceScaleFactor; 
		RP_Global_Float mAdaptiveKernelSizeRayHitDistanceScaleExponent; 
	

		RP_Global_Float mValueSigma;
		RP_Global_Float mNormalSigma;
		RP_Global_Int mMinKernelWidth;
		RP_Global_Float mKernelWidthPercentage;
		RP_Global_Float mMinVarianceToDenoise;
		RP_Global_Float mDepthWeightCufoff;


		RP_Global_Int mTspBlurPasses;


		SharedPtr<TemporalSupersampling_ReverseReproject>	   mTemporalSupersamplingReverseReproject;
		SharedPtr<TemporalSupersampling_BlendWithCurrentFrame> mTemporalSupersampling_BlendWithCurrentFrame;
		SharedPtr<AtrousWaveletTransformCrossBilateralFilter>  mAtrousWaveletTransformCrossBilateralFilter;
		SharedPtr<DisocclusionBilateralFilter>	mDisocclusionBilateralFilter;
		SharedPtr<CalculateMeanVariance>		mCalculateMeanVariance;
		SharedPtr<ITexture> mTemporalCache[2][ETemporalSuperSampling::Count];
		SharedPtr<ITexture> mTemporalValue[2];
		SharedPtr<ITexture> mCachedTsppValueSquaredValueRayHitDistance;
		SharedPtr<ITexture> mVarianceResources;
		SharedPtr<ITexture> mLocalMeanVarianceResources;
		SharedPtr<ITexture> mDisocclusionBlurStrength;
		SharedPtr<ITexture> mPrevFrameGBufferNormalDepth;

		u32 mTemporalCacheCurrentFrameResourceIndex = 0;
		u32 mTemporalCacheCurrentFrameTemporalValueResourceIndex = 0;
	public:
		Denoiser(const String& name, Renderer* renderer);
		Output Execute(SharedPtr<IComputeContext> context, const Input& input);
	private:
		void Init();
		void InitTexture();
	private:
		String GetRenderParamName(const String& name);
	};

}