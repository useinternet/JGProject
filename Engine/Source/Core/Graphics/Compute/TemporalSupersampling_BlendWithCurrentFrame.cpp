#include "pch.h"
#include "TemporalSupersampling_BlendWithCurrentFrame.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/RootSignature.h"
#include "Graphics/GraphicsHelper.h"

namespace JG
{
	TemporalSupersampling_BlendWithCurrentFrame::TemporalSupersampling_BlendWithCurrentFrame(Renderer* renderer)
	{
		mRenderer = renderer;
		mShader = ShaderLibrary::GetInstance().FindComputeShader("TemporalSupersampling_BlendWithCurrentFrame");
	}
	void TemporalSupersampling_BlendWithCurrentFrame::Execute(SharedPtr<IComputeContext> context, const Input& input)
	{
		if (mShader == nullptr)
		{
			return;
		}


		context->BindShader(mShader);
		context->BindRootSignature(mRenderer->GetComputeRootSignature());

		struct CB
		{
			f32 StdDevGamma;
			f32 Clamping_minStdDevTolerance;
			f32 ClampDifferenceToTsppScale;
			f32 MinSmoothingFactor;

			u32 MinTsppToUseTemporalVariance;
			u32 BlurStrength_MaxTspp;
			f32 BlurDecayStrength;
		};
		CB CB;
		CB.StdDevGamma = input.StdDevGamma;
		CB.Clamping_minStdDevTolerance = input.ClampingMinStdDevTolerance;
		CB.ClampDifferenceToTsppScale  = input.ClampDifferenceToTsppScale;
		CB.MinSmoothingFactor = input.MinSmoothingFactor;
		CB.MinTsppToUseTemporalVariance = input.MinTsppToUseTemporalVariance;
		CB.BlurStrength_MaxTspp = input.BlurStrength_MaxTspp;
		CB.BlurDecayStrength = input.BlurDecayStrength;

		context->BindConstantBuffer((u32)Renderer::EComputeRootParam::CB0, CB);

		context->BindTextures((u32)Renderer::EComputeRootParam::SRV, { 
			input.Value,
			input.LocalMeanVariance,
			input.RayHitDistance,
			input.Reprojected_Tspp_Value_SquaredMeanValue_RayHitDistance});

		context->BindTextures((u32)Renderer::EComputeRootParam::UAV,{
			input.OutValue,
			input.OutTspp,
			input.OutSquaredMeanValue,
			input.OutRayHitDistance,
			input.OutVariance,
			input.OutBlurStrength
			});

		context->Dispatch2D(input.Resolution.x, input.Resolution.y);
	}
}