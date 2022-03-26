#include "pch.h"
#include "Denoiser.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/RootSignature.h"
#include "Graphics/GraphicsHelper.h"
#include "Graphics/Compute/TemporalSupersampling_ReverseReproject.h"
#include "Graphics/Compute/TemporalSupersampling_BlendWithCurrentFrame.h"
#include "Graphics/Compute/AtrousWaveletTransformCrossBilateralFilter.h"
#include "Graphics/Compute/DisocclusionBilateralFilter.h"
#include "Graphics/Compute/CalculateMeanVariance.h"
namespace JG
{
	Denoiser::Denoiser(Renderer* renderer)
	{
		mRenderer = renderer;
		Init();
	}
	Denoiser::Output Denoiser::Execute(SharedPtr<IComputeContext> context, const Input& input)
	{
		if (mResolution != input.Resolution)
		{
			mResolution = input.Resolution;
			InitTexture();
		}

		u32 bufferIndex = JGGraphics::GetInstance().GetBufferIndex();
		
		u32 temporalCachePreviousFrameResourceIndex = mTemporalCacheCurrentFrameResourceIndex;
		mTemporalCacheCurrentFrameResourceIndex = (mTemporalCacheCurrentFrameResourceIndex + 1) % 2;

		u32 temporalCachePreviousFrameTemporalValueResourceIndex = mTemporalCacheCurrentFrameTemporalValueResourceIndex;
		mTemporalCacheCurrentFrameTemporalValueResourceIndex = (mTemporalCacheCurrentFrameTemporalValueResourceIndex + 1) % 2;

		context->ClearUAVFloat(mDebugTextures[bufferIndex]);
		// TemporalSupersampling_ReverseReproject
		{
			TemporalSupersampling_ReverseReproject::Input _input;
			_input.Resolution = input.Resolution;
			_input.CurrentFrameNormalDepth = input.NormalDepth;
			_input.CurrentFrameDepthDerivative = input.DepthDerivatives;
			_input.ReprojectedNormalDepth = input.ReprojectedNormalDepth;
			_input.MotionVector = input.MotionVector;
			_input.CachedValue  = mTemporalValue[temporalCachePreviousFrameTemporalValueResourceIndex];
			_input.CachedNormalDepth = mPrevFrameGBufferNormalDepth;
			_input.CachedTspp = mTemporalCache[temporalCachePreviousFrameResourceIndex][ETemporalSuperSampling::Tspp];
			_input.CachedValueSquareMean = mTemporalCache[temporalCachePreviousFrameResourceIndex][ETemporalSuperSampling::CoefficientSquaredMean];
			_input.CachedRayHitDepth = mTemporalCache[temporalCachePreviousFrameResourceIndex][ETemporalSuperSampling::RayHitDistance];
			_input.OutCachedTspp = mTemporalCache[mTemporalCacheCurrentFrameResourceIndex][ETemporalSuperSampling::Tspp];
			_input.OutReprojectedCachedValues = mCachedTsppValueSquaredValueRayHitDistance;
			_input.OutDebug = mDebugTextures[bufferIndex];
			_input.DepthSigma = mDepthSigma.GetValue();
			mTemporalSupersamplingReverseReproject->Execute(context, _input);


			context->TransitionBarrier(
				{
					mTemporalCache[mTemporalCacheCurrentFrameResourceIndex][ETemporalSuperSampling::Tspp],
					mCachedTsppValueSquaredValueRayHitDistance
				},
				{
					EResourceState::NonePixelShaderResource,
					EResourceState::NonePixelShaderResource
				});
			context->UAVBarrier(
				{ 
					mTemporalCache[mTemporalCacheCurrentFrameResourceIndex][ETemporalSuperSampling::Tspp] 
				});

			SharedPtr<ICopyContext> copyContext = context->QueryInterfaceAsCopyContext();

			copyContext->CopyTextureRegion(
				mPrevFrameGBufferNormalDepth, input.NormalDepth,
				JRect(0, 0, mResolution.x, mResolution.y),
				EResourceState::Common, EResourceState::Common);

		}
		mDebug.SetValue(mDebugTextures[bufferIndex]);
		//CalculateMeanVariance
		{
			context->TransitionBarrier({ input.Value }, { EResourceState::NonePixelShaderResource });
			context->UAVBarrier({ input.Value });


			CalculateMeanVariance::Input _input;
			_input.Resolution = mResolution;
			_input.KernelWidth = mVariance_BilateralFilterKernelWidth.GetValue();
			_input.Value = input.Value;
			_input.OutMeanVariance = mLocalMeanVarianceResources[EVarianceResource::Raw];
			
			mCalculateMeanVariance->Execute(context, _input);


			context->TransitionBarrier(
				{ mLocalMeanVarianceResources[EVarianceResource::Raw] }, { EResourceState::NonePixelShaderResource });
			context->UAVBarrier({ mLocalMeanVarianceResources[EVarianceResource::Raw] });
		
		}
		// TemporalSupersampling_BlendWithCurrentFrame
		{
			context->TransitionBarrier({
				mTemporalValue[mTemporalCacheCurrentFrameTemporalValueResourceIndex],
				mTemporalCache[mTemporalCacheCurrentFrameResourceIndex][ETemporalSuperSampling::Tspp],
				mTemporalCache[mTemporalCacheCurrentFrameResourceIndex][ETemporalSuperSampling::CoefficientSquaredMean],
				mTemporalCache[mTemporalCacheCurrentFrameResourceIndex][ETemporalSuperSampling::RayHitDistance],
				mVarianceResources[EVarianceResource::Raw],
				mDisocclusionBlurStrength
				}, {
				EResourceState::UnorderedAccess,
				EResourceState::UnorderedAccess,
				EResourceState::UnorderedAccess,
				EResourceState::UnorderedAccess,
				EResourceState::UnorderedAccess,
				EResourceState::UnorderedAccess
				});
			context->UAVBarrier({ mCachedTsppValueSquaredValueRayHitDistance });

			TemporalSupersampling_BlendWithCurrentFrame::Input _input;
			_input.Resolution = mResolution;
			_input.Value = input.Value;
			_input.LocalMeanVariance = mLocalMeanVarianceResources[EVarianceResource::Raw];
			_input.RayHitDistance = input.RayHitDistance;
			_input.Reprojected_Tspp_Value_SquaredMeanValue_RayHitDistance = mCachedTsppValueSquaredValueRayHitDistance;
			_input.OutValue = mTemporalValue[mTemporalCacheCurrentFrameTemporalValueResourceIndex];
			_input.OutTspp = mTemporalCache[mTemporalCacheCurrentFrameResourceIndex][ETemporalSuperSampling::Tspp];
			_input.OutSquaredMeanValue = mTemporalCache[mTemporalCacheCurrentFrameResourceIndex][ETemporalSuperSampling::CoefficientSquaredMean];
			_input.OutRayHitDistance = mTemporalCache[mTemporalCacheCurrentFrameResourceIndex][ETemporalSuperSampling::RayHitDistance];
			_input.OutVariance = mVarianceResources[EVarianceResource::Raw];
			_input.OutBlurStrength = mDisocclusionBlurStrength;
			
			_input.StdDevGamma = mStdDevGamma.GetValue();
			_input.ClampingMinStdDevTolerance = mClampingMinStdDevTolerance.GetValue();
			_input.ClampDifferenceToTsppScale = mClampDifferenceToTsppScale.GetValue();
			_input.MinSmoothingFactor = 1.f / mMaxTspp.GetValue();
			_input.MinTsppToUseTemporalVariance = mMinTsppToUseTemporalVariance.GetValue();
			_input.BlurStrength_MaxTspp = mBlurStrengthMaxTspp.GetValue();
			_input.BlurDecayStrength = mBlurDecayStrength.GetValue();

			mTemporalSupersampling_BlendWithCurrentFrame->Execute(context, _input);

			context->TransitionBarrier({
				mTemporalCache[mTemporalCacheCurrentFrameResourceIndex][ETemporalSuperSampling::Tspp],
				mTemporalCache[mTemporalCacheCurrentFrameResourceIndex][ETemporalSuperSampling::CoefficientSquaredMean],
				mTemporalCache[mTemporalCacheCurrentFrameResourceIndex][ETemporalSuperSampling::RayHitDistance],
				mVarianceResources[EVarianceResource::Raw],
				mDisocclusionBlurStrength,
				mTemporalValue[mTemporalCacheCurrentFrameTemporalValueResourceIndex]
				}, {
					EResourceState::NonePixelShaderResource,
					EResourceState::NonePixelShaderResource,
					EResourceState::NonePixelShaderResource,
					EResourceState::NonePixelShaderResource,
					EResourceState::NonePixelShaderResource,
					EResourceState::NonePixelShaderResource
				});

			context->UAVBarrier({ 
				mTemporalCache[mTemporalCacheCurrentFrameResourceIndex][ETemporalSuperSampling::Tspp],
				mTemporalCache[mTemporalCacheCurrentFrameResourceIndex][ETemporalSuperSampling::CoefficientSquaredMean],
				mTemporalCache[mTemporalCacheCurrentFrameResourceIndex][ETemporalSuperSampling::RayHitDistance],
				mVarianceResources[EVarianceResource::Raw],
				mDisocclusionBlurStrength,
				mTemporalValue[mTemporalCacheCurrentFrameTemporalValueResourceIndex] });
		}



		{
			SharedPtr<ITexture> Variance = mVarianceResources[EVarianceResource::Raw];

			// Adaptive kernel radius rotation.
			float kernelRadiusLerfCoef = 0;
			if (mKernelRadiusRotateKernelEnable.GetValue())
			{
				u32 numCycles = mKernelRadiusRotateKernelNumCycles.GetValue();
				static u32 frameID = 0;

				u32 i = frameID++ % numCycles;
				kernelRadiusLerfCoef = (float)i / static_cast<float>(numCycles);
			}
			SharedPtr<ITexture> inputValue = mTemporalValue[mTemporalCacheCurrentFrameTemporalValueResourceIndex];
			mTemporalCacheCurrentFrameTemporalValueResourceIndex = (mTemporalCacheCurrentFrameTemporalValueResourceIndex + 1) % 2;
			SharedPtr<ITexture> outputValue = mTemporalValue[mTemporalCacheCurrentFrameTemporalValueResourceIndex];

			context->TransitionBarrier({ outputValue }, { EResourceState::UnorderedAccess });


			f32 RayHitDistanceScaleFactor = 22 / input.MaxRayDistance * mAdaptiveKernelSizeRayHitDistanceScaleFactor.GetValue();
			f32 RayHitDistanceScaleExponent = Math::Lerp<f32>(1, mAdaptiveKernelSizeRayHitDistanceScaleExponent.GetValue(), Math::RelativeCoef<f32>(input.MaxRayDistance, 4, 22));

			AtrousWaveletTransformCrossBilateralFilter::Input _input;
			_input.Resolution = mResolution;
			_input.Value	   = inputValue;
			_input.NormalDepth = input.NormalDepth;
			_input.Variance	   = Variance;
			_input.HitDistance = mTemporalCache[mTemporalCacheCurrentFrameResourceIndex][ETemporalSuperSampling::RayHitDistance];
			_input.PartialDistanceDerivatives = input.DepthDerivatives;
			_input.Tspp				= mTemporalCache[mTemporalCacheCurrentFrameResourceIndex][ETemporalSuperSampling::Tspp];
			_input.OutFilteredValue = outputValue;
			_input.ValueSigma = mValueSigma.GetValue();
			_input.DepthSigma = mDepthSigma.GetValue();
			_input.NormalSigma = mNormalSigma.GetValue();
			_input.KernelRadiusLerfCoef = kernelRadiusLerfCoef;
			_input.RayHitDistanceScaleFactor = RayHitDistanceScaleFactor;
			_input.RayHitDistanceScaleExponent = RayHitDistanceScaleExponent;
			_input.MinKernelWidth = mMinKernelWidth.GetValue();
			_input.MaxKernelWidth = (f32)(mKernelWidthPercentage.GetValue() / 100) * (f32)mResolution.x;
			_input.MinVarianceToDenoise = mMinVarianceToDenoise.GetValue();
			_input.DepthWeightCutoff    = mDepthWeightCufoff.GetValue();

			mAtrousWaveletTransformCrossBilateralFilter->Execute(context, _input);


			context->TransitionBarrier({ outputValue }, { EResourceState::NonePixelShaderResource });
		}
		{
			SharedPtr<ITexture> inOutResource = mTemporalValue[mTemporalCacheCurrentFrameTemporalValueResourceIndex];
			context->TransitionBarrier({ inOutResource }, { EResourceState::UnorderedAccess });
			u32 filterStep = 1;
			u32 numPasses = mTspBlurPasses.GetValue();

			for (u32 i = 0; i < numPasses; ++i)
			{
				context->UAVBarrier({ inOutResource });
				DisocclusionBilateralFilter::Input _input;
				_input.Resolution = mResolution;
				_input.Depth = input.Depth;
				_input.BlurStrength = mDisocclusionBlurStrength;
				_input.OutValue = inOutResource;
				_input.Step = filterStep;
				mDisocclusionBilateralFilter->Execute(context, _input);
					
				filterStep *= 2;
			}
			context->TransitionBarrier({ inOutResource }, { EResourceState::NonePixelShaderResource });
			context->UAVBarrier({ inOutResource });
		}
		Output output;
		output.OutValue = mTemporalValue[mTemporalCacheCurrentFrameTemporalValueResourceIndex];
		return output;
	}
	void Denoiser::Init()
	{
		mTemporalSupersamplingReverseReproject = CreateSharedPtr<TemporalSupersampling_ReverseReproject>(mRenderer);
		mTemporalSupersampling_BlendWithCurrentFrame = CreateSharedPtr<TemporalSupersampling_BlendWithCurrentFrame>(mRenderer);
		mAtrousWaveletTransformCrossBilateralFilter = CreateSharedPtr<AtrousWaveletTransformCrossBilateralFilter>(mRenderer);
		mCalculateMeanVariance = CreateSharedPtr<CalculateMeanVariance>(mRenderer);
		mDisocclusionBilateralFilter = CreateSharedPtr<DisocclusionBilateralFilter>(mRenderer);
		mDepthSigma = RP_Global_Float::Create("Renderer/Denoiser/DepthSigma", 1.0f, 0.0f, 10.0f, mRenderer->GetRenderParamManager());
		mVariance_BilateralFilterKernelWidth = RP_Global_Int::Create("Renderer/Denoiser/Variance_KernelWidth", 9, 3, 9, mRenderer->GetRenderParamManager());
		mMaxTspp = RP_Global_Int::Create("Renderer/Denoiser/MaxTspp", 33, 1, 100, mRenderer->GetRenderParamManager());


		mStdDevGamma = RP_Global_Float::Create("Renderer/Denoiser/StdDevGamma", 0.6f, 0.1f, 10.0f, mRenderer->GetRenderParamManager());
		mClampingMinStdDevTolerance = RP_Global_Float::Create("Renderer/Denoiser/ClampingMinStdDevTolerance", 0.05f, 0.0f, 1.0f, mRenderer->GetRenderParamManager());
		mClampDifferenceToTsppScale = RP_Global_Float::Create("Renderer/Denoiser/ClampDifferenceToTsppScale", 4.0f, 0.0f, 10.0f, mRenderer->GetRenderParamManager());

		mMinTsppToUseTemporalVariance = RP_Global_Int::Create("Renderer/Denoiser/MinTsppToUseTemporalVariance", 4, 1, 40, mRenderer->GetRenderParamManager());
		mBlurStrengthMaxTspp = RP_Global_Int::Create("Renderer/Denoiser/BlurStrengthMaxTspp", 12, 0, 100, mRenderer->GetRenderParamManager());
		mBlurDecayStrength = RP_Global_Float::Create("Renderer/Denoiser/BlurDecayStrength", 1.0f, 0.1f, 32.0f, mRenderer->GetRenderParamManager());



		mKernelRadiusRotateKernelEnable = RP_Global_Bool::Create("Renderer/Denosier/KernelRadiusRotateKernelEnable", true, mRenderer->GetRenderParamManager());
		mKernelRadiusRotateKernelNumCycles = RP_Global_Int::Create("Renderer/Denoiser/KernelRadiusRotateKernelNumCycles", 3, 1, 10, mRenderer->GetRenderParamManager());
		mAdaptiveKernelSizeRayHitDistanceScaleFactor = RP_Global_Float::Create("Renderer/Denoiser/AdaptiveKernelSizeRayHitDistanceScaleFactor", 0.02f, 0.001f, 0.1f, mRenderer->GetRenderParamManager());
		mAdaptiveKernelSizeRayHitDistanceScaleExponent = RP_Global_Float::Create("Renderer/Denoiser/AdaptiveKernelSizeRayHitDistanceScaleExponent", 2.0f, 1.0f, 5.0f, mRenderer->GetRenderParamManager());


		mValueSigma = RP_Global_Float::Create("Renderer/Denoiser/ValueSigma", 1.0f, 0.0f, 30.0f, mRenderer->GetRenderParamManager());
		mNormalSigma = RP_Global_Float::Create("Renderer/Denoiser/NormalSigma", 64, 0, 256, mRenderer->GetRenderParamManager());
		mMinKernelWidth = RP_Global_Int::Create("Renderer/Denoiser/MinKernelWidth", 3, 3, 101, mRenderer->GetRenderParamManager());
		mKernelWidthPercentage = RP_Global_Float::Create("Renderer/Denoiser/KernelWidthPercentage", 1.5f, 0, 100, mRenderer->GetRenderParamManager());
		mMinVarianceToDenoise = RP_Global_Float::Create("Renderer/Denoiser/MinVarianceToDenoise", 0.0F, 0.0F, 1.0F, mRenderer->GetRenderParamManager());
		mDepthWeightCufoff = RP_Global_Float::Create("Renderer/Denoiser/DepthWeightCufoff", 0.2F, 0.0F, 2.0F, mRenderer->GetRenderParamManager());

		mDebug = RP_Global_Tex::Create("Debug", nullptr, mRenderer->GetRenderParamManager());
		mTspBlurPasses = RP_Global_Int::Create("Renderer/Denoiser/TspBlurPasses ", 3, 0, 6, mRenderer->GetRenderParamManager());
	}
	void Denoiser::InitTexture()
	{
		TextureInfo texInfo;
		texInfo.Width = std::max<u32>(1, mResolution.x);
		texInfo.Height = std::max<u32>(1, mResolution.y);
		texInfo.ArraySize = 1;
		texInfo.Format = ETextureFormat::R32G32B32A32_Float;
		texInfo.Flags = ETextureFlags::Allow_UnorderedAccessView | ETextureFlags::Allow_RenderTarget;
		texInfo.MipLevel = 1;
		texInfo.ClearColor = Color();
		GraphicsHelper::InitRenderTextures(texInfo, "Debug", &mDebugTextures);
		for (i32 i = 0; i < 2; ++i)
		{
			texInfo.Format = ETextureFormat::R8_Uint;
			mTemporalCache[i][ETemporalSuperSampling::Tspp] = ITexture::Create("Tspp", texInfo);

			texInfo.Format = ETextureFormat::R16_Float;
			mTemporalCache[i][ETemporalSuperSampling::CoefficientSquaredMean] = ITexture::Create("CoefficientSquaredMean" + std::to_string(i), texInfo);
			mTemporalCache[i][ETemporalSuperSampling::RayHitDistance] = ITexture::Create("CoefficientSquaredMean" + std::to_string(i), texInfo);
			mTemporalValue[i] = ITexture::Create("TemporalValue" + std::to_string(i), texInfo);
		}
		texInfo.Format = ETextureFormat::R16G16B16A16_Uint;
		mCachedTsppValueSquaredValueRayHitDistance = ITexture::Create("CachedTsppValueSquaredValueRayHitDistance", texInfo);

		texInfo.Format = ETextureFormat::R16_Float;
		mVarianceResources[EVarianceResource::Raw] = ITexture::Create("Variance_Raw", texInfo);
		mVarianceResources[EVarianceResource::Smoothed] = ITexture::Create("Variance_Smoothed", texInfo);

		texInfo.Format = ETextureFormat::R16G16_Float;
		mLocalMeanVarianceResources[EVarianceResource::Raw] = ITexture::Create("LocalMeanVariance_Raw", texInfo);
		mLocalMeanVarianceResources[EVarianceResource::Smoothed] = ITexture::Create("LocalMeanVariance_Smoothed", texInfo);


		texInfo.Format = ETextureFormat::R8_Unorm;
		mDisocclusionBlurStrength = ITexture::Create("DisocclustionStrength", texInfo);

		texInfo.Format = ETextureFormat::R32_Uint;
		mPrevFrameGBufferNormalDepth = ITexture::Create("PrevFrameNormalDepth", texInfo);

	}
}