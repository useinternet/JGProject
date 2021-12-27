#include "pch.h"
#include "PostProcess_ToneMapping.h"




namespace JG
{
	PostProcess_ToneMapping::PostProcess_ToneMapping()
	{



	}

	void PostProcess_ToneMapping::Awake(Renderer* renderer)
	{
		renderer->RegisterProcessShaderParam<PostProcess_ToneMapping>(MATERIAL_PARAM_EXPOSURE, sizeof(f32));
		renderer->SetProcessShaderParam<PostProcess_ToneMapping, f32>(MATERIAL_PARAM_EXPOSURE, mExposure);


		renderer->RegisterProcessShaderParam<PostProcess_ToneMapping>("Enable", sizeof(bool));
		renderer->SetProcessShaderParam<PostProcess_ToneMapping, bool>("Enable", mIsEnable);
	}

	void PostProcess_ToneMapping::Ready(Renderer* renderer, IGraphicsAPI* api, Graphics::RenderPassData* renderPassData, const RenderInfo& info)
	{
		if (mSceneMaterial == nullptr)
		{
			SharedPtr<IGraphicsShader> shader = ShaderLibrary::GetInstance().FindGraphicsShader(ShaderDefine::Template::StandardSceneShader, { SCRIPT_NAME });
			if (shader->IsSuccessed() == true)
			{
				mSceneMaterial = IMaterial::Create("ToneMapping", shader);
				mSceneMaterial->SetDepthStencilState(EDepthStencilStateTemplate::NoDepth);
			}
		}
		if (mPrevResolution != info.Resolution)
		{
			mPrevResolution = info.Resolution;
			u64 bufferCnt = JGGraphics::GetInstance().GetBufferCount();
		

			mTargetTextures.resize(bufferCnt);
			InitTexture(info.Resolution);
		}

		renderer->GetProcessShaderParam<PostProcess_ToneMapping, f32>(MATERIAL_PARAM_EXPOSURE, &mExposure);
		renderer->GetProcessShaderParam<PostProcess_ToneMapping, bool>("Enable", &mIsEnable);
	}

	void PostProcess_ToneMapping::Run(Renderer* renderer, IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result)
	{
		if (mSceneMaterial == nullptr || mSceneMaterial->IsValid() == false || mIsEnable == false)
		{
			return;
		}

		if (result == nullptr || result->SceneTexture == nullptr || result->SceneTexture->IsValid() == false)
		{
			return;
		}


		auto commandID     = JGGraphics::GetInstance().RequestCommandID();
		auto targetTexture = mTargetTextures[info.CurrentBufferIndex];


		api->SetViewports(commandID, { Viewport(info.Resolution.x, info.Resolution.y) });
		api->SetScissorRects(commandID, { ScissorRect(0,0, info.Resolution.x,info.Resolution.y) });
		api->ClearRenderTarget(commandID, { targetTexture }, nullptr);
		api->SetRenderTarget(commandID, { targetTexture }, nullptr);

		if (mSceneMaterial->SetFloat(MATERIAL_PARAM_EXPOSURE, mExposure) == false)
		{
			return;
		}
		if (mSceneMaterial->SetTexture(MATERIAL_PARAM_SCENETEXTURE, result->SceneTexture) == false)
		{
			return;
		}

		if (mSceneMaterial->Bind(commandID) == false)
		{
			return;
		}

		api->Draw(commandID, 6);

		result->SceneTexture = targetTexture;
	}

	bool PostProcess_ToneMapping::IsCompelete()
	{
		return true;
	}

	Type PostProcess_ToneMapping::GetType() const
	{
		return JGTYPE(PostProcess_ToneMapping);
	}
	void PostProcess_ToneMapping::InitTexture(const JVector2& size)
	{
		TextureInfo mainTexInfo;
		mainTexInfo.Width = std::max<u32>(1, size.x);
		mainTexInfo.Height = std::max<u32>(1, size.y);
		mainTexInfo.ArraySize = 1;
		mainTexInfo.Format = ETextureFormat::R16G16B16A16_Float;
		mainTexInfo.Flags = ETextureFlags::Allow_RenderTarget;
		mainTexInfo.MipLevel = 1;

		i32 index = 0;
		for (auto& t : mTargetTextures)
		{
			if (t == nullptr) t = ITexture::Create("_TargetTexture_" + std::to_string(index), mainTexInfo);
			else t->SetTextureInfo(mainTexInfo);
			++index;

		}
	}
}

