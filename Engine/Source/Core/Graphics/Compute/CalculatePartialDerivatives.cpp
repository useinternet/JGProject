#include "pch.h"
#include "CalculatePartialDerivatives.h"
#include "Graphics/RootSignature.h"
#include "Graphics/Renderer.h"
#include "Graphics/Resource.h"
#include "Graphics/Shader.h"
#include "Graphics/JGGraphics.h"
namespace JG
{



	CalculatePartialDerivatives::CalculatePartialDerivatives(Renderer* renderer)
	{
		mRenderer = renderer;
		mShader = ShaderLibrary::GetInstance().FindComputeShader("CalculatePartialDerivatives");
		
	}

	void CalculatePartialDerivatives::Execute(SharedPtr<IComputeContext> context, const Input& input)
	{
		if (mShader == nullptr)
		{
			return;
		}

		
		// Shader
		//
		u32 currIndex = JGGraphics::GetInstance().GetBufferIndex();

		context->BindShader(mShader);
		context->BindRootSignature(mRenderer->GetComputeRootSignature());

		struct CB
		{
			JVector2Uint Resolution;
		};
		CB CB;
		CB.Resolution.x = input.Resolution.x;
		CB.Resolution.y = input.Resolution.y;

		context->BindConstantBuffer((u32)Renderer::EComputeRootParam::CB0, CB);
		context->BindTextures((u32)Renderer::EComputeRootParam::SRV, { input.Input });
		context->BindTextures((u32)Renderer::EComputeRootParam::UAV, { input.Output });
		context->Dispatch2D(input.Resolution.x, input.Resolution.y);
	}

}