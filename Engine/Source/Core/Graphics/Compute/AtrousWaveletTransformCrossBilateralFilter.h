#pragma once

#include "JGCore.h"


namespace JG
{
	class IComputeContext;
	class IRootSignature;
	class ITexture;
	class Renderer;
	class IComputeShader;
	class AtrousWaveletTransformCrossBilateralFilter
	{
	public:
		struct Input
		{
			JVector2 Resolution;
			SharedPtr<ITexture> Value;
			SharedPtr<ITexture> NormalDepth;
			SharedPtr<ITexture> Variance;
			SharedPtr<ITexture> HitDistance;
			SharedPtr<ITexture> PartialDistanceDerivatives;
			SharedPtr<ITexture> Tspp;
			SharedPtr<ITexture> OutFilteredValue;

			f32 ValueSigma; // 1.0f, 0.0f, 30.0f
			f32 DepthSigma; // 1.0f, 0.0f, 10.0f
			f32 NormalSigma; // 64, 0, 256
			f32 KernelRadiusLerfCoef;
			f32 RayHitDistanceScaleFactor;
			f32 RayHitDistanceScaleExponent;
			u32 MinKernelWidth; // 3, 3, 101, WidthPercentage 1.5f, 0, 100
			u32 MaxKernelWidth;
			f32 MinVarianceToDenoise; // 0.0f, 0.0f, 1.f
			f32 DepthWeightCutoff; // 0.2f, 0.0f, 2.0f
		};
	private:
		Renderer* mRenderer;
		SharedPtr<IComputeShader> mShader;
	public:
		AtrousWaveletTransformCrossBilateralFilter(Renderer* renderer);
		void Execute(SharedPtr<IComputeContext> context, const Input& input);
	};
}