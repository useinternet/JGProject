#include "pch.h"
#include "TemporalSupersampling_ReverseReproject.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/RootSignature.h"
#include "Graphics/GraphicsHelper.h"
namespace JG
{
	TemporalSupersampling_ReverseReproject::TemporalSupersampling_ReverseReproject(Renderer* renderer)
	{
		mRenderer = renderer;
		mShader = ShaderLibrary::GetInstance().FindComputeShader("TemporalSupersampling_ReverseReproject");

	}

	void TemporalSupersampling_ReverseReproject::Execute(SharedPtr<IComputeContext> context, const Input& input)
	{
		if (mShader == nullptr)
		{
			return;
		}


		context->BindShader(mShader);
		context->BindRootSignature(mRenderer->GetComputeRootSignature());

		struct CB
		{
			JVector2Uint  TextureDim;
			JVector2      InvTextureDim;

			f32   DepthSigma;
			u32   DepthNumMantissaBits;      
		};
		CB CB;
		CB.TextureDim.x = input.Resolution.x;
		CB.TextureDim.y = input.Resolution.y;
		CB.InvTextureDim.x = 1.0f / input.Resolution.x;
		CB.InvTextureDim.y = 1.0f / input.Resolution.y;
		CB.DepthSigma = input.DepthSigma;
		CB.DepthNumMantissaBits = GraphicsHelper::NumMantissaBitsInFloatFormat(16);

		context->BindConstantBuffer((u32)Renderer::EComputeRootParam::CB0, CB);

		context->BindTextures((u32)Renderer::EComputeRootParam::SRV,
			{
			input.CurrentFrameNormalDepth,
			input.CurrentFrameDepthDerivative,
			input.ReprojectedNormalDepth,
			input.MotionVector,
			input.CachedNormalDepth,
			input.CachedValue,
			input.CachedTspp,
			input.CachedValueSquareMean,
			input.CachedRayHitDepth});

		context->BindTextures((u32)Renderer::EComputeRootParam::UAV, 
			{ input.OutCachedTspp, input.OutReprojectedCachedValues, input.OutDebug });

		context->Dispatch2D(input.Resolution.x, input.Resolution.y);

	}

}