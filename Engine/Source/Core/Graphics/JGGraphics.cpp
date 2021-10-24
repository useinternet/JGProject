#include "pch.h"
#include "JGGraphics.h"


namespace JG
{









	JGGraphics::JGGraphics()
	{
	}

	JGGraphics::~JGGraphics()
	{
	}

	Graphics::Scene* JGGraphics::CreateScene(const String& name)
	{
		return nullptr;
	}

	IGraphicsAPI* JGGraphics::GetGraphicsAPI() const
	{
		return mGraphcisAPI.get();
	}




}


namespace JG
{
	namespace Graphics
	{
		void GObject::SetName(const String& name)
		{
			mName = name;
		}

		void GObject::SetLayer(u64 layer)
		{
			mLayer = layer;
		}

		const String& GObject::GetName()
		{
			return mName;
		}

		u64 GObject::GetLayer()
		{
			return mLayer;
		}



		Scene::Scene(const SceneInfo& info)
		{
			auto bufferCount = JGGraphics::GetInstance().GetGraphicsAPI()->GetBufferCount();
			mTargetTextures.resize(bufferCount, nullptr);
			mTargetDepthTextures.resize(bufferCount, nullptr);
			CurrentIndex = 0;

			m2DBatch = CreateSharedPtr<Render2DBatch>();
			SetSceneInfo(info);
		}

		void Scene::SetSceneInfo(const SceneInfo& info)
		{
			if (mRenderer == nullptr || mSceneInfo.RenderPath != info.RenderPath)
			{
				InitRenderer(info.RenderPath);
			}
			if (mTargetTextures.empty() || mSceneInfo.Resolution != info.Resolution)
			{
				InitTexture(info.Resolution, info.ClearColor);
			}
			mSceneInfo = info;
		}

		const SceneInfo& Scene::GetSceneInfo() const
		{
			return mSceneInfo;
		}
		void Scene::PushSceneObject(SceneObject* object)
		{

		}
		void Scene::PushLight(Light* light)
		{

		}


		void Scene::InitRenderer(ERendererPath path)
		{
			auto bufferCount = JGGraphics::GetInstance().GetGraphicsAPI()->GetBufferCount();
			switch (path)
			{
			case ERendererPath::Foward:
				mRenderer = CreateSharedPtr<FowardRenderer>();
				break;
			case ERendererPath::Deferred:
				break;
			}
		}

		void Scene::InitTexture(const JVector2& resolution, const Color& clearColor)
		{
			TextureInfo mainTexInfo;
			mainTexInfo.Width     = std::max<u32>(1, resolution.x);
			mainTexInfo.Height    = std::max<u32>(1, resolution.y);
			mainTexInfo.ArraySize = 1;
			mainTexInfo.Format    = ETextureFormat::R32G32B32A32_Float;
			mainTexInfo.Flags     = ETextureFlags::Allow_RenderTarget;
			mainTexInfo.MipLevel   = 1;
			mainTexInfo.ClearColor = clearColor;

			for (auto& t : mTargetTextures)
			{
				if (t == nullptr) t = ITexture::Create(GetName() + "_TargetTexture", mainTexInfo);
				else t->SetTextureInfo(mainTexInfo);

			}

			mainTexInfo.Format = ETextureFormat::D24_Unorm_S8_Uint;
			mainTexInfo.Flags  = ETextureFlags::Allow_DepthStencil;
			for (auto& t : mTargetDepthTextures)
			{
				if (t == nullptr) t = ITexture::Create(GetName() + "_TargetDepthTexture", mainTexInfo);
				else t->SetTextureInfo(mainTexInfo);

			}
		}

		void SceneObject::SetWorldMatrix(const JMatrix& m)
		{
			mWorldMatrix = m;
		}

		const JMatrix& SceneObject::GetWorldMatrix() const
		{
			return mWorldMatrix;
		}





		void Render2DObject::SetColor(const Color& color)
		{
			mColor = color;
		}
		void Render2DObject::SetTexture(SharedPtr<ITexture> tex)
		{
			mTexture = tex;
		}


		const Color& Render2DObject::GetColor() const
		{

		}
		SharedPtr<ITexture> Render2DObject::GetTexture() const
		{

		}



	}
}

