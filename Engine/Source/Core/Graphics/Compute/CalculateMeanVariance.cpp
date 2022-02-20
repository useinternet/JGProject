#include "pch.h"
#include "CalculateMeanVariance.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/RootSignature.h"
#include "Graphics/GraphicsHelper.h"
namespace JG
{



	CalculateMeanVariance::CalculateMeanVariance(Renderer* renderer)
	{
		mRenderer = renderer;
		mShader = ShaderLibrary::GetInstance().FindComputeShader("CalculateMeanVariance");

	}

	void CalculateMeanVariance::Execute(SharedPtr<IComputeContext> context, const Input& input)
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
			u32 KernelWidth;
			u32 KernelRadius;
			u32 PixelStepY;
		};
		CB CB;
		CB.TextureDim.x = input.Resolution.x;
		CB.TextureDim.y = input.Resolution.y;
		CB.KernelWidth  = input.KernelWidth;
		CB.KernelRadius = CB.KernelWidth >> 1;
		CB.PixelStepY = 1;

		context->BindConstantBuffer((u32)Renderer::EComputeRootParam::CB0, CB);
		
		context->BindTextures((u32)Renderer::EComputeRootParam::SRV,
			{input.Value});

		context->BindTextures((u32)Renderer::EComputeRootParam::UAV,
			{ input.OutMeanVariance });

		context->Dispatch2D(input.Resolution.x, input.Resolution.y);

	}

}