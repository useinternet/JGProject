#include "pch.h"
#include "FloatAccumulater.h"
#include "Graphics/Shader.h"
#include "Graphics/GraphicsHelper.h"
#include "Graphics/GraphicsAPI.h"
#include "Graphics/Renderer.h"
#include "Graphics/JGGraphics.h"
namespace JG
{
	namespace Compute
	{
		FloatAccumulater::FloatAccumulater(Renderer* renderer)
		{
			mRenderer = renderer;
			mShader = ShaderLibrary::GetInstance().FindComputeShader("FloatAccumulate");






		}
		FloatAccumulater::Output FloatAccumulater::Execute(SharedPtr<IComputeContext> context, const FloatAccumulater::Input& input)
		{
			if (mShader == nullptr)
			{
				return FloatAccumulater::Output();
			}
			InitTextures(input.Resolution);


			u32 currIndex = JGGraphics::GetInstance().GetBufferIndex();

			context->BindShader(mShader);
			context->BindRootSignature(mRenderer->GetComputeRootSignature());


			// prev, cur
			if (input.IsResetAccumCount)
			{
				mCB.AccumCount = 0;
			}

			context->BindConstantBuffer((u32)Renderer::EComputeRootParam::CB0, mCB);
			context->BindTextures((u32)Renderer::EComputeRootParam::SRV, { mPrevs[currIndex], input.CurrFrame });
			context->BindTextures((u32)Renderer::EComputeRootParam::UAV, { mResults[currIndex] });
			context->Dispatch2D(mResolution.x, mResolution.y);

			FloatAccumulater::Output output;
			output.Result = mResults[currIndex];


			Swap(mResults[currIndex], mPrevs[currIndex]);
			mCB.AccumCount++;
			return output;
		}
		void FloatAccumulater::ResetAccumCount()
		{
			mCB.AccumCount = 0;
		}
		void FloatAccumulater::InitTextures(const JVector2& resolution)
		{
			if (mResolution == resolution)
			{
				return;
			}
			mResolution = resolution;
			mCB.AccumCount = 0;
			TextureInfo texInfo;
			texInfo.Width = std::max<u32>(1, mResolution.x);
			texInfo.Height = std::max<u32>(1, mResolution.y);
			texInfo.ArraySize = 1;
			texInfo.Format = ETextureFormat::R32_Float;
			texInfo.Flags = ETextureFlags::Allow_UnorderedAccessView;
			texInfo.MipLevel = 1;
			texInfo.ClearColor = Color();

			GraphicsHelper::InitRenderTextures(texInfo, "PrevAccumTexture", &mPrevs);
			GraphicsHelper::InitRenderTextures(texInfo, "ResultAccumTexture", &mResults);


		}
		void FloatAccumulater::Swap(SharedPtr<ITexture>& t1, SharedPtr<ITexture>& t2)
		{
			SharedPtr<ITexture> temp = t1;
			t1 = t2;
			t2 = temp;
		}
	}

}