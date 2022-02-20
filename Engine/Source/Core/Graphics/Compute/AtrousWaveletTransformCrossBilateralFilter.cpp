#include "pch.h"
#include "AtrousWaveletTransformCrossBilateralFilter.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/RootSignature.h"
#include "Graphics/GraphicsHelper.h"

namespace JG
{
	AtrousWaveletTransformCrossBilateralFilter::AtrousWaveletTransformCrossBilateralFilter(Renderer* renderer)
	{
		mRenderer = renderer;
		mShader   = ShaderLibrary::GetInstance().FindComputeShader("AtrousWaveletTransformCrossBilateralFilter");
	}
	void AtrousWaveletTransformCrossBilateralFilter::Execute(SharedPtr<IComputeContext> context, const Input& input)
	{
		if (mShader == nullptr)
		{
			return;
		}


		context->BindShader(mShader);
		context->BindRootSignature(mRenderer->GetComputeRootSignature());

		struct CB
		{
			JVector2Uint TextureDim;
			f32 DepthWeightCutoff;
			f32 KernelRadiusLerfCoef;
			u32 MinKernelWidth;
			u32 MaxKernelWidth;

			f32 RayHitDistanceToKernelWidthScale;
			f32 RayHitDistanceToKernelSizeScaleExponent;
			f32 MinVarianceToDenoise;

			f32 ValueSigma;
			f32 DepthSigma;
			f32 NormalSigma;
			u32 DepthNumMantissaBits;
		};
		CB CB;
		CB.TextureDim = JVector2Uint(input.Resolution.x, input.Resolution.y);
		CB.DepthWeightCutoff = input.DepthWeightCutoff;
		CB.KernelRadiusLerfCoef = input.KernelRadiusLerfCoef;
		CB.MinKernelWidth = input.MinKernelWidth;
		CB.MaxKernelWidth = input.MaxKernelWidth;
		CB.RayHitDistanceToKernelWidthScale = input.RayHitDistanceScaleFactor;
		CB.RayHitDistanceToKernelSizeScaleExponent = input.RayHitDistanceScaleExponent;
		CB.MinVarianceToDenoise = input.MinVarianceToDenoise;
		CB.ValueSigma = input.ValueSigma;
		CB.DepthSigma = input.DepthSigma;
		CB.NormalSigma = input.NormalSigma;
		CB.DepthNumMantissaBits = GraphicsHelper::NumMantissaBitsInFloatFormat(16);


		context->BindConstantBuffer((u32)Renderer::EComputeRootParam::CB0, CB);

		context->BindTextures((u32)Renderer::EComputeRootParam::SRV, {
			 input.Value,
			input.NormalDepth,
			input.Variance,
			input.HitDistance,
			input.PartialDistanceDerivatives,
			input.Tspp});

		context->BindTextures((u32)Renderer::EComputeRootParam::UAV, {
			input.OutFilteredValue
			});

		context->Dispatch2D(input.Resolution.x, input.Resolution.y, 16 , 16);
	}
}