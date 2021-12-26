#include "pch.h"
#include "PostProcess_Bloom.h"




namespace JG
{
	PostProcess_Bloom::PostProcess_Bloom()
	{
	}

	void PostProcess_Bloom::Ready(Renderer* renderer, IGraphicsAPI* api, Graphics::RenderPassData* rednerPassData, const RenderInfo& info)
	{

		InitMaterial();

		if (mPrevResolution != info.Resolution)
		{

			mPrevResolution = info.Resolution;

			

			InitTexture(info.Resolution);
		}

	}

	void PostProcess_Bloom::Run(Renderer* renderer, IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result)
	{
		auto commandID = JGGraphics::GetInstance().RequestCommandID();
		mSceneTexture = result->SceneTexture;

		mBrightnessTexture = Run_Brightness(commandID, info);

		for (int i = 0; i < 10; ++i)
		{
			mBrightnessTexture = Run_BlurH(commandID, info);
			mBrightnessTexture = Run_BlurV(commandID, info);
		}

		result->SceneTexture = Run_Bloom(commandID, info);
	}

	bool PostProcess_Bloom::IsCompelete()
	{
		return true;
	}

	Type PostProcess_Bloom::GetType() const
	{
		return JGTYPE(PostProcess_Bloom);
	}
	SharedPtr<ITexture> PostProcess_Bloom::Run_Brightness(u64 commandID, const RenderInfo& info)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();

		if (mBrightnessData.Material != nullptr && mBrightnessData.Material->IsValid())
		{

			SharedPtr<ITexture> targetTexture = mBrightnessData.Textures[info.CurrentBufferIndex];


			api->SetViewports(commandID, { Viewport(mDownSamplingResolution.x, mDownSamplingResolution.y) });
			api->SetScissorRects(commandID, { ScissorRect(0,0, mDownSamplingResolution.x, mDownSamplingResolution.y) });
			api->SetRenderTarget(commandID, { targetTexture }, nullptr);

			if (mBrightnessData.Material->SetTexture("SceneTexture", mSceneTexture) == false)
			{
				return nullptr;
			}

			if (mBrightnessData.Material->Bind(commandID) == false)
			{
				return nullptr;
			}

			api->Draw(commandID, 6);
			return targetTexture;
		}
		return nullptr;
	}
	SharedPtr<ITexture> PostProcess_Bloom::Run_BlurH(u64 commandID, const RenderInfo& info)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();

		if (mBlurHData.Material != nullptr && mBlurHData.Material->IsValid())
		{

			SharedPtr<ITexture> targetTexture = mBlurHData.Textures[info.CurrentBufferIndex];


			api->SetViewports(commandID, { Viewport(mDownSamplingResolution.x, mDownSamplingResolution.y) });
			api->SetScissorRects(commandID, { ScissorRect(0,0, mDownSamplingResolution.x, mDownSamplingResolution.y) });
			api->SetRenderTarget(commandID, { targetTexture }, nullptr);

			if (mBlurHData.Material->SetTexture("SceneTexture", mBrightnessTexture) == false)
			{
				return nullptr;
			}
			
			if (mBlurHData.Material->Bind(commandID) == false)
			{
				return nullptr;
			}

			api->Draw(commandID, 6);
			return targetTexture;
		}
		return nullptr;
	}
	SharedPtr<ITexture> PostProcess_Bloom::Run_BlurV(u64 commandID, const RenderInfo& info)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();

		if (mBlurVData.Material != nullptr && mBlurVData.Material->IsValid())
		{

			SharedPtr<ITexture> targetTexture = mBlurVData.Textures[info.CurrentBufferIndex];


			api->SetViewports(commandID, { Viewport(mDownSamplingResolution.x, mDownSamplingResolution.y) });
			api->SetScissorRects(commandID, { ScissorRect(0,0, mDownSamplingResolution.x, mDownSamplingResolution.y) });
			api->SetRenderTarget(commandID, { targetTexture }, nullptr);

			if (mBlurVData.Material->SetTexture("SceneTexture", mBrightnessTexture) == false)
			{
				return nullptr;
			}

			if (mBlurVData.Material->Bind(commandID) == false)
			{
				return nullptr;
			}

			api->Draw(commandID, 6);
			return targetTexture;
		}
		return nullptr;
	}
	SharedPtr<ITexture> PostProcess_Bloom::Run_Bloom(u64 commandID, const RenderInfo& info)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();

		if (mBloomData.Material != nullptr && mBloomData.Material->IsValid())
		{

			SharedPtr<ITexture> targetTexture = mBloomData.Textures[info.CurrentBufferIndex];


			api->SetViewports(commandID, { Viewport(info.Resolution.x, info.Resolution.y) });
			api->SetScissorRects(commandID, { ScissorRect(0,0, info.Resolution.x, info.Resolution.y) });
			api->SetRenderTarget(commandID, { targetTexture }, nullptr);

			if (mBloomData.Material->SetTexture("SceneTexture", mSceneTexture) == false)
			{
				return nullptr;
			}
			if (mBloomData.Material->SetTexture("BrightnessTexture", mBrightnessTexture) == false)
			{
				return nullptr;
			}
			if (mBloomData.Material->Bind(commandID) == false)
			{
				return nullptr;
			}

			api->Draw(commandID, 6);
			return targetTexture;
		}
		return nullptr;
	}
	void PostProcess_Bloom::InitMaterial()
	{
		if (mBrightnessData.Material == nullptr)
		{
			SharedPtr<IGraphicsShader> shader = ShaderLibrary::GetInstance().FindGraphicsShader(ShaderDefine::Template::StandardSceneShader, { "Scene/Brightness" });
			if (shader != nullptr && shader->IsSuccessed())
			{
				mBrightnessData.Material = IMaterial::Create("Brightness", shader);
				mBrightnessData.Material->SetDepthStencilState(EDepthStencilStateTemplate::NoDepth);
			}
		}

		if (mBlurHData.Material == nullptr)
		{
			SharedPtr<IGraphicsShader> shader = ShaderLibrary::GetInstance().FindGraphicsShader(ShaderDefine::Template::StandardSceneShader, { "Scene/GaussianBlur_H" });
			if (shader != nullptr && shader->IsSuccessed())
			{
				mBlurHData.Material = IMaterial::Create("GaussianBlur_H", shader);
				mBlurHData.Material->SetDepthStencilState(EDepthStencilStateTemplate::NoDepth);
			}
		}
		if (mBlurVData.Material == nullptr)
		{
			SharedPtr<IGraphicsShader> shader = ShaderLibrary::GetInstance().FindGraphicsShader(ShaderDefine::Template::StandardSceneShader, { "Scene/GaussianBlur_V" });
			if (shader != nullptr && shader->IsSuccessed())
			{
				mBlurVData.Material = IMaterial::Create("GaussianBlur_V", shader);
				mBlurVData.Material->SetDepthStencilState(EDepthStencilStateTemplate::NoDepth);
			}
		}
		if (mBloomData.Material == nullptr)
		{
			SharedPtr<IGraphicsShader> shader = ShaderLibrary::GetInstance().FindGraphicsShader(ShaderDefine::Template::StandardSceneShader, { "Scene/Bloom" });
			if (shader != nullptr && shader->IsSuccessed())
			{
				mBloomData.Material = IMaterial::Create("Bloom", shader);
				mBloomData.Material->SetDepthStencilState(EDepthStencilStateTemplate::NoDepth);
			}
		}
	}
	void PostProcess_Bloom::InitTexture(const JVector2& resolution)
	{
		auto bufferCnt = JGGraphics::GetInstance().GetBufferCount();

		mDownSamplingResolution = resolution;

		TextureInfo texInfo;
		texInfo.Width     = std::max<u32>(1, mDownSamplingResolution.x);
		texInfo.Height    = std::max<u32>(1, mDownSamplingResolution.y);
		texInfo.ArraySize = 1;
		texInfo.Format    = ETextureFormat::R16G16B16A16_Float;
		texInfo.Flags     = ETextureFlags::Allow_RenderTarget;
		texInfo.MipLevel  = 1;



		{
			i32 index = 0;
			mBrightnessData.Textures.resize(bufferCnt);
			for (auto& t : mBrightnessData.Textures)
			{
				if (t == nullptr) t = ITexture::Create("Bloom_Brightness_Texture_" + std::to_string(index), texInfo);
				else t->SetTextureInfo(texInfo);
				++index;
			}
		}

		{
			i32 index = 0;
			mBlurHData.Textures.resize(bufferCnt);
			for (auto& t : mBlurHData.Textures)
			{
				if (t == nullptr) t = ITexture::Create("Blur_H_Texture_" + std::to_string(index), texInfo);
				else t->SetTextureInfo(texInfo);
				++index;
			}
		}
		{
			i32 index = 0;
			mBlurVData.Textures.resize(bufferCnt);
			for (auto& t : mBlurVData.Textures)
			{
				if (t == nullptr) t = ITexture::Create("Blur_V_Texture_" + std::to_string(index), texInfo);
				else t->SetTextureInfo(texInfo);
				++index;
			}
		}

		{
			i32 index = 0;
			mBloomData.Textures.resize(bufferCnt);
			texInfo.Width = resolution.x;
			texInfo.Height = resolution.y;
			for (auto& t : mBloomData.Textures)
			{
				if (t == nullptr) t = ITexture::Create("Bloom_Texture_" + std::to_string(index), texInfo);
				else t->SetTextureInfo(texInfo);
				++index;
			}
		}

	}
}

