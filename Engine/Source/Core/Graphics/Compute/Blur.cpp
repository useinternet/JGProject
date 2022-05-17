#include "pch.h"
#include "Blur.h"
#include "Graphics/Shader.h"
#include "Graphics/GraphicsHelper.h"
#include "Graphics/GraphicsAPI.h"
#include "Graphics/Renderer.h"
#include "Graphics/JGGraphics.h"


namespace JG
{


	namespace Compute
	{
		Blur_Float::Blur_Float(Renderer* renderer)
		{
			mRenderer = renderer;
			mShader = ShaderLibrary::GetInstance().FindComputeShader("Blur_Float");

		}
		Blur_Float::Output Blur_Float::Execute(SharedPtr<IComputeContext> context, const Input& input)
		{

			if (mShader == nullptr)
			{
				return Blur_Float::Output();
			}

			context->BindShader(mShader);
			context->BindRootSignature(mRenderer->GetComputeRootSignature());


			context->BindTextures((u32)Renderer::EComputeRootParam::SRV, { input.Src });
			context->BindTextures((u32)Renderer::EComputeRootParam::UAV, { input.Src });
			context->Dispatch2D(input.Resolution.x, input.Resolution.y);

			Blur_Float::Output output;
			output.Result = input.Src;
			return output;
		}
	}



}