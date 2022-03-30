#include "pch.h"
#include "RT_Composite.h"
#include "Graphics/RootSignature.h"
#include "Graphics/Renderer.h"
#include "Graphics/Resource.h"
#include "Graphics/Shader.h"
#include "Graphics/JGGraphics.h"


namespace JG
{
	RT_Composite::RT_Composite(Renderer* renderer)
	{
		mRenderer = renderer;
		mShader = ShaderLibrary::GetInstance().FindComputeShader("RT_Composite");

	}

	void RT_Composite::Execute(SharedPtr<IComputeContext> context, const Input& input)
	{
		if (mShader == nullptr)
		{
			return;
		}

		context->BindShader(mShader);
		context->BindRootSignature(mRenderer->GetComputeRootSignature());
		context->BindConstantBuffer((u32)Renderer::EComputeRootParam::CB0, input.FarZ);
		context->BindTextures((u32)Renderer::EComputeRootParam::SRV, {
			input.Direct,
			input.IndirectR,
			input.IndirectG,
			input.IndirectB,
			input.Shadow,
			input.AO,
			input.Depth
			});
		context->BindTextures((u32)Renderer::EComputeRootParam::UAV, { input.Output });
		context->Dispatch2D(input.Resolution.x, input.Resolution.y);
	}
}