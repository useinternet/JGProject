#include "pch.h"
#include "DisocclusionBilateralFilter.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/RootSignature.h"
#include "Graphics/GraphicsHelper.h"

namespace JG
{
	DisocclusionBilateralFilter::DisocclusionBilateralFilter(Renderer* renderer)
	{
		mRenderer = renderer;
		mShader = ShaderLibrary::GetInstance().FindComputeShader("DisocclusionBilateralFilter");
	}
	void DisocclusionBilateralFilter::Execute(SharedPtr<IComputeContext> context, const Input& input)
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
			u32 Step;
		};
		CB CB;
		CB.TextureDim.x = input.Resolution.x;
		CB.TextureDim.y = input.Resolution.y;
		CB.Step = input.Step;
		context->BindConstantBuffer((u32)Renderer::EComputeRootParam::CB0, CB);

		context->BindTextures((u32)Renderer::EComputeRootParam::SRV, {
			input.Depth,
			input.BlurStrength
 });

		context->BindTextures((u32)Renderer::EComputeRootParam::UAV, {
	input.OutValue
			});

		context->Dispatch2D(input.Resolution.x, input.Resolution.y);
	}
}