#include "pch.h"
#include "PostProcess_Bloom.h"




namespace JG
{
	PostProcess_Bloom::PostProcess_Bloom()
	{
	}

	void PostProcess_Bloom::Awake(Renderer* renderer)
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

		List<SharedPtr<ITexture>> brightnessTextures;
		brightnessTextures.resize(mDownSamplingCount);


		f32 k = 1.0f;
		for (i32 i = 0; i < mDownSamplingCount; ++i)
		{
			k *= 0.5f;
			JVector2 resolution = info.Resolution * k;
			
			brightnessTextures[i] = Run_Brightness(commandID, info, i, resolution, result->SceneTexture);

			for (i32 j = 0; j < 10; ++j)
			{
				brightnessTextures[i] = Run_BlurH(commandID, info, i, resolution, brightnessTextures[i]);
				brightnessTextures[i] = Run_BlurV(commandID, info, i, resolution, brightnessTextures[i]);
			}
		}
		

		//for (i32 i = mDownSamplingCount - 1; i >= 0; --i)
		//{

		//	result->SceneTexture = Run_Bloom(commandID, info,i, result->SceneTexture, brightnessTextures[i]);
		//}

		result->SceneTexture = brightnessTextures[0];

	}

	bool PostProcess_Bloom::IsCompelete()
	{
		return true;
	}

	Type PostProcess_Bloom::GetType() const
	{
		return JGTYPE(PostProcess_Bloom);
	}
	SharedPtr<ITexture> PostProcess_Bloom::Run_Brightness(u64 commandID, const RenderInfo& info, i32 index, const JVector2& resolution, SharedPtr<ITexture> sceneTexture)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();

		if (mBrightnessMaterial != nullptr && mBrightnessMaterial->IsValid())
		{
			SharedPtr<ITexture> targetTexture = mBrightnessTextures[index][info.CurrentBufferIndex];
			api->SetViewports(commandID, { Viewport(resolution.x, resolution.y) });
			api->SetScissorRects(commandID, { ScissorRect(0,0, resolution.x, resolution.y) });
			api->ClearRenderTarget(commandID, { targetTexture }, nullptr);
			api->SetRenderTarget(commandID, { targetTexture }, nullptr);

			if (mBrightnessMaterial->SetTexture("SceneTexture", sceneTexture) == false)
			{
				return nullptr;
			}

			if (mBrightnessMaterial->Bind(commandID) == false)
			{
				return nullptr;
			}

			api->Draw(commandID, 6);
			return targetTexture;
		}
		return nullptr;
	}
	SharedPtr<ITexture> PostProcess_Bloom::Run_BlurH(u64 commandID, const RenderInfo& info, i32 index, const JVector2& resolution, SharedPtr<ITexture> sceneTexture)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();

		if (mBlurHMaterial != nullptr && mBlurHMaterial->IsValid())
		{

			SharedPtr<ITexture> targetTexture   = mBlurHTextures[index][info.CurrentBufferIndex];

			api->SetViewports(commandID, { Viewport(resolution.x, resolution.y) });
			api->ClearRenderTarget(commandID, { targetTexture }, nullptr);
			api->SetScissorRects(commandID, { ScissorRect(0,0, resolution.x, resolution.y) });
			api->SetRenderTarget(commandID, { targetTexture }, nullptr);

			if (mBlurHMaterial->SetTexture("SceneTexture", sceneTexture) == false)
			{
				return nullptr;
			}
			
			if (mBlurHMaterial->Bind(commandID) == false)
			{
				return nullptr;
			}

			api->Draw(commandID, 6);
			return targetTexture;
		}
		return nullptr;
	}
	SharedPtr<ITexture> PostProcess_Bloom::Run_BlurV(u64 commandID, const RenderInfo& info, i32 index, const JVector2& resolution, SharedPtr<ITexture> sceneTexture)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();

		if (mBlurVMaterial != nullptr && mBlurVMaterial->IsValid())
		{

			SharedPtr<ITexture> targetTexture     = mBlurVTextures[index][info.CurrentBufferIndex];

			api->SetViewports(commandID, { Viewport(resolution.x, resolution.y) });
			api->ClearRenderTarget(commandID, { targetTexture }, nullptr);
			api->SetScissorRects(commandID, { ScissorRect(0,0, resolution.x, resolution.y) });
			api->SetRenderTarget(commandID, { targetTexture }, nullptr);

			if (mBlurVMaterial->SetTexture("SceneTexture", sceneTexture) == false)
			{
				return nullptr;
			}

			if (mBlurVMaterial->Bind(commandID) == false)
			{
				return nullptr;
			}

			api->Draw(commandID, 6);
			return targetTexture;
		}
		return nullptr;
	}
	SharedPtr<ITexture> PostProcess_Bloom::Run_Bloom(u64 commandID, const RenderInfo& info, i32 index, SharedPtr<ITexture> sceneTexture, SharedPtr<ITexture> brightnessTexture)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();

		if (mBloomData.Material != nullptr && mBloomData.Material->IsValid())
		{

			SharedPtr<ITexture> targetTexture     = mBloomData.Textures[info.CurrentBufferIndex];

			api->SetViewports(commandID, { Viewport(info.Resolution.x, info.Resolution.y) });
			api->ClearRenderTarget(commandID, { targetTexture }, nullptr);
			api->SetScissorRects(commandID, { ScissorRect(0,0, info.Resolution.x, info.Resolution.y) });
			api->SetRenderTarget(commandID, { targetTexture }, nullptr);

			if (mBloomData.Material->SetTexture("SceneTexture", sceneTexture) == false)
			{
				return nullptr;
			}
			if (mBloomData.Material->SetTexture("BrightnessTexture", brightnessTexture) == false)
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
		//if (mBrightnessData.Material == nullptr)
		//{
		//	SharedPtr<IGraphicsShader> shader = ShaderLibrary::GetInstance().FindGraphicsShader(ShaderDefine::Template::StandardSceneShader, { "Scene/Brightness" });
		//	if (shader != nullptr && shader->IsSuccessed())
		//	{
		//		mBrightnessData.Material = IMaterial::Create("Brightness", shader);
		//		mBrightnessData.Material->SetDepthStencilState(EDepthStencilStateTemplate::NoDepth);
		//	}
		//}

		//if (mBlurHData.Material == nullptr)
		//{
		//	SharedPtr<IGraphicsShader> shader = ShaderLibrary::GetInstance().FindGraphicsShader(ShaderDefine::Template::StandardSceneShader, { "Scene/GaussianBlur_H" });
		//	if (shader != nullptr && shader->IsSuccessed())
		//	{
		//		mBlurHData.Material = IMaterial::Create("GaussianBlur_H", shader);
		//		mBlurHData.Material->SetDepthStencilState(EDepthStencilStateTemplate::NoDepth);
		//	}
		//}
		//if (mBlurVData.Material == nullptr)
		//{
		//	SharedPtr<IGraphicsShader> shader = ShaderLibrary::GetInstance().FindGraphicsShader(ShaderDefine::Template::StandardSceneShader, { "Scene/GaussianBlur_V" });
		//	if (shader != nullptr && shader->IsSuccessed())
		//	{
		//		mBlurVData.Material = IMaterial::Create("GaussianBlur_V", shader);
		//		mBlurVData.Material->SetDepthStencilState(EDepthStencilStateTemplate::NoDepth);
		//	}
		//}

		if(mBrightnessMaterial == nullptr)
		{
			SharedPtr<IGraphicsShader> shader = ShaderLibrary::GetInstance().FindGraphicsShader(ShaderDefine::Template::StandardSceneShader, { "Scene/Brightness" });
			if (shader != nullptr && shader->IsSuccessed())
			{
				mBrightnessMaterial = IMaterial::Create("Brightness", shader);
				mBrightnessMaterial->SetDepthStencilState(EDepthStencilStateTemplate::NoDepth);
			}
		}

		if (mBlurHMaterial == nullptr)
		{
			SharedPtr<IGraphicsShader> shader = ShaderLibrary::GetInstance().FindGraphicsShader(ShaderDefine::Template::StandardSceneShader, { "Scene/GaussianBlur_H" });
			if (shader != nullptr && shader->IsSuccessed())
			{
				mBlurHMaterial = IMaterial::Create("GaussianBlur_H", shader);
				mBlurHMaterial->SetDepthStencilState(EDepthStencilStateTemplate::NoDepth);
			}
		}


		if (mBlurVMaterial == nullptr)
		{
			SharedPtr<IGraphicsShader> shader = ShaderLibrary::GetInstance().FindGraphicsShader(ShaderDefine::Template::StandardSceneShader, { "Scene/GaussianBlur_V" });
			if (shader != nullptr && shader->IsSuccessed())
			{
				mBlurVMaterial = IMaterial::Create("GaussianBlur_V", shader);
				mBlurVMaterial->SetDepthStencilState(EDepthStencilStateTemplate::NoDepth);
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

		TextureInfo texInfo;
		texInfo.Width     = std::max<u32>(1, resolution.x);
		texInfo.Height    = std::max<u32>(1, resolution.y);
		texInfo.ArraySize = 1;
		texInfo.Format    = ETextureFormat::R16G16B16A16_Float;
		texInfo.Flags     = ETextureFlags::Allow_RenderTarget;
		texInfo.MipLevel  = 1;



		//{
		//	i32 index = 0;
		//	mBrightnessData.Textures.resize(bufferCnt);
		//	for (auto& t : mBrightnessData.Textures)
		//	{
		//		if (t == nullptr) t = ITexture::Create("Bloom_Brightness_Texture_" + std::to_string(index), texInfo);
		//		else t->SetTextureInfo(texInfo);
		//		++index;
		//	}
		//}

		//{
		//	i32 index = 0;
		//	mBlurHData.Textures.resize(bufferCnt);
		//	for (auto& t : mBlurHData.Textures)
		//	{
		//		if (t == nullptr) t = ITexture::Create("Blur_H_Texture_" + std::to_string(index), texInfo);
		//		else t->SetTextureInfo(texInfo);
		//		++index;
		//	}
		//}
		//{
		//	i32 index = 0;
		//	mBlurVData.Textures.resize(bufferCnt);
		//	for (auto& t : mBlurVData.Textures)
		//	{
		//		if (t == nullptr) t = ITexture::Create("Blur_V_Texture_" + std::to_string(index), texInfo);
		//		else t->SetTextureInfo(texInfo);
		//		++index;
		//	}
		//}

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




		f32 k = 1.0F;
		mBlurHTextures.resize(mDownSamplingCount);
		mBlurVTextures.resize(mDownSamplingCount);
		mBrightnessTextures.resize(mDownSamplingCount);
		for (i32 i = 0; i < mDownSamplingCount; ++i)
		{



			auto& h_texlist = mBlurHTextures[i];
			h_texlist.resize(bufferCnt);

			auto& v_texlist = mBlurVTextures[i];
			v_texlist.resize(bufferCnt);

			auto& b_texlist = mBrightnessTextures[i];
			b_texlist.resize(bufferCnt);

			k *= 0.5F;
			f32 w = resolution.x * k;
			f32 h = resolution.y * k;

			texInfo.Width = w;
			texInfo.Height = h;
			//texInfo.Width  = resolution.x * 0.5f;
			//texInfo.Height = resolution.y * 0.5f;
			for (auto& t : h_texlist)
			{
				if (t == nullptr) t = ITexture::Create("Blur_HTexture", texInfo);
				else t->SetTextureInfo(texInfo);
			}

			for (auto& t : v_texlist)
			{
				if (t == nullptr) t = ITexture::Create("Blur_VTexture", texInfo);
				else t->SetTextureInfo(texInfo);
			}



			for (auto& t : b_texlist)
			{
				if (t == nullptr) t = ITexture::Create("Brigtness_Texture", texInfo);
				else t->SetTextureInfo(texInfo);
			}
		}
	}
}

