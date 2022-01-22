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
			InitTextures(input.Resolution, input.TextureFormat);


			u32 currIndex = JGGraphics::GetInstance().GetBufferIndex();

			context->BindShader(mShader);
			context->BindRootSignature(mRenderer->GetComputeRootSignature());


			context->BindTextures((u32)Renderer::EComputeRootParam::SRV, { input.Src });
			context->BindTextures((u32)Renderer::EComputeRootParam::UAV, { mResults[currIndex] });
			context->Dispatch2D(mResolution.x, mResolution.y);

			Blur_Float::Output output;
			output.Result = mResults[currIndex];
			return output;
		}
		void Blur_Float::InitTextures(const JVector2& resolution, ETextureFormat format)
		{

			if (mResolution == resolution && mTextureFormat == format)
			{
				return;
			}
			mResolution = resolution;
			mTextureFormat = format;
			TextureInfo texInfo;
			texInfo.Width = std::max<u32>(1, mResolution.x);
			texInfo.Height = std::max<u32>(1, mResolution.y);
			texInfo.ArraySize = 1;
			texInfo.Format = mTextureFormat;
			texInfo.Flags = ETextureFlags::Allow_UnorderedAccessView;
			texInfo.MipLevel = 1;
			texInfo.ClearColor = Color();

			GraphicsHelper::InitRenderTextures(texInfo, "BlurResult", &mResults);
		}
	}



}