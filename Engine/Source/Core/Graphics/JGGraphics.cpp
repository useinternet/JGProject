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
		return CreateGObject<Graphics::Scene>(name);
	}

	Graphics::StaticRenderObject* JGGraphics::CreateStaticRenderObject(const String& name)
	{
		return CreateGObject<Graphics::StaticRenderObject>(name);
	}

	Graphics::PointLight* JGGraphics::CreatePointLight(const String& name)
	{
		return CreateGObject<Graphics::PointLight>(name);
	}

	void JGGraphics::DestroyObject(Graphics::GObject* gobject)
	{
		if (mGraphcisAPI == nullptr)
		{
			return;
		}
		Scheduler::GetInstance().ScheduleOnceByFrame(mGraphcisAPI->GetBufferCount(), SchedulePriority::BeginSystem,
			[&]() -> EScheduleResult
		{




			return EScheduleResult::Continue;
		});
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

		void GObject::Lock()
		{
			mLock = true;
		}

		void GObject::UnLock()
		{
			mLock = false;
		}

		bool GObject::IsLock()
		{
			return mLock;
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



		void StaticRenderObject::SetMesh(SharedPtr<IMesh> mesh)
		{
			mMesh = mesh;
		}

		void StaticRenderObject::SetMaterialList(const List<SharedPtr<IMaterial>> materialList)
		{
			mMaterialList = materialList;
		}

		SharedPtr<IMesh> StaticRenderObject::GetMesh() const
		{
			return mMesh;
		}

		const List<SharedPtr<IMaterial>> StaticRenderObject::GetMaterialList() const
		{
			return mMaterialList;
		}

		bool StaticRenderObject::IsValid() const
		{
			bool result = true;

			if (mMesh == nullptr || mMesh->IsValid() == false)
			{
				result = false;
			}
			if (mMaterialList.empty())
			{
				result = false;
			}
			else
			{
				for (auto& m : mMaterialList)
				{
					if (m == nullptr)
					{
						result = false;
						break;
					}
				}
			}

			return result;
		}

		void PointLight::PushBtData(List<jbyte>& btData)
		{
			PushData(btData, &mPosition, sizeof(JVector3));
			PushData(btData, &mRange, sizeof(float));
			PushData(btData, &mColor, sizeof(JVector3));
			PushData(btData, &mIntensity, sizeof(float));
			PushData(btData, &mAtt0, sizeof(float));
			PushData(btData, &mAtt1, sizeof(float));
			PushData(btData, &mAtt2, sizeof(float));
		}
		void PointLight::SetColor(const Color& color)
		{
			mColor = JVector3(color.R, color.G, color.B);
		}
		void PointLight::SetPosition(const JVector3& position)
		{
			mPosition = position;
		}
		void PointLight::SetIntensity(f32 intensity)
		{
			mIntensity = intensity;
		}
		void PointLight::SetRange(f32 range)
		{
			mRange = range;
		}
		void PointLight::SetAtt0(f32 att0)
		{
			mAtt0 = att0;
		}
		void PointLight::SetAtt1(f32 att1)
		{
			mAtt1 = att1;
		}
		void PointLight::SetAtt2(f32 att2)
		{
			mAtt2 = att2;
		}


		Color PointLight::GetColor(const Color& color)
		{
			return Color(mColor.x, mColor.y, mColor.z, 1.0f);
		}
		const JVector3& PointLight::GetPosition() const
		{
			return mPosition;
		}
		f32 PointLight::GetIntensity() const
		{
			return mIntensity;
		}
		f32 PointLight::GetRange() const
		{
			return mRange;
		}
		f32 PointLight::GetAtt0() const
		{
			return mAtt0;
		}
		f32 PointLight::GetAtt1() const
		{
			return mAtt1;
		}
		f32 PointLight::GetAtt2() const
		{
			return mAtt2;
		}
}
}

