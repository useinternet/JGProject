#pragma once

#include "JGCore.h"


namespace JG
{
	class IComputeContext;
	class IRootSignature;
	class ITexture;
	class Renderer;
	class IComputeShader;
	class TemporalSupersampling_BlendWithCurrentFrame
	{
	public:
		struct Input
		{
			JVector2 Resolution;

			f32 StdDevGamma;
			f32 ClampingMinStdDevTolerance;
			f32 ClampDifferenceToTsppScale;
			f32 MinSmoothingFactor;
			u32 MinTsppToUseTemporalVariance;
			u32 BlurStrength_MaxTspp;
			u32 BlurDecayStrength;


			SharedPtr<ITexture> Value;
			SharedPtr<ITexture> LocalMeanVariance;
			SharedPtr<ITexture> RayHitDistance;
			SharedPtr<ITexture> Reprojected_Tspp_Value_SquaredMeanValue_RayHitDistance;
			SharedPtr<ITexture> OutValue;
			SharedPtr<ITexture> OutTspp;
			SharedPtr<ITexture> OutSquaredMeanValue;
			SharedPtr<ITexture> OutRayHitDistance;
			SharedPtr<ITexture> OutVariance;
			SharedPtr<ITexture> OutBlurStrength;
		};
	private:

		Renderer* mRenderer;
		SharedPtr<IComputeShader> mShader;

	public:
		TemporalSupersampling_BlendWithCurrentFrame(Renderer* renderer);
		void Execute(SharedPtr<IComputeContext> context, const Input& input);


	};
}