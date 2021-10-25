#pragma once
#include "JGCore.h"
#include "GraphicsDefine.h"
#include "GraphicsAPI.h"
#include "Resource.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Renderer.h"






namespace JG
{
	namespace Graphics
	{
		class Scene;
		class GObject;
		class RenderObject;
		class StaticRenderObject;
		class Light;
		class PointLight;
	}
	class JGGraphics : public GlobalSingleton<JGGraphics>
	{
		UniquePtr<IGraphicsAPI> mGraphcisAPI;
		Dictionary<Graphics::GObject*, UniquePtr<Graphics::GObject>> mObjectPool;
	public:
		JGGraphics();
		~JGGraphics();


		Graphics::Scene*			  CreateScene(const String& name);
		Graphics::StaticRenderObject* CreateStaticRenderObject(const String& name);
		Graphics::PointLight*         CreatePointLight(const String& name);


		void DestroyObject(Graphics::GObject* gobject);
		IGraphicsAPI* GetGraphicsAPI() const;
	private:
		template<class T>
		T* CreateGObject(const String& name)
		{
			auto gobj = CreateUniquePtr<T>();
			gobj->SetName(name);
			
			auto result = gobj.get();
			mObjectPool[gobj.get()] = std::move(gobj);
			return result;
		}
		void RemoveObject(Graphics::GObject* gobj)
		{
			mObjectPool.erase(gobj);
		}
	private:
		//class 
	};
}

namespace JG
{
	namespace Graphics
	{
		class SceneInfo
		{
		public:
			ERendererPath RenderPath = ERendererPath::Deferred;
			JVector2 Resolution;

			
			JVector3 EyePos;
			JMatrix  ViewMatrix = JMatrix::Identity();
			JMatrix  ProjMatrix = JMatrix::Identity();
			JMatrix  ViewProjMatrix = JMatrix::Identity();

			Color    ClearColor;
		};


		class GObject
		{
		private:
			String mName;
			u64    mLayer;
			bool   mLock = false;
		public:
			virtual ~GObject() = default;
		public:

			void SetName(const String& name);
			void SetLayer(u64 layer);


			const String& GetName();
			u64 GetLayer();
		protected:
			void Lock();
			void UnLock();
			bool IsLock();
		};


		//
		class Scene : public GObject
		{
			SceneInfo mSceneInfo;

			List<SharedPtr<ITexture>> mTargetTextures;
			List<SharedPtr<ITexture>> mTargetDepthTextures;
			u64 CurrentIndex = 0;

			SharedPtr<Renderer>      mRenderer;
			SharedPtr<Render2DBatch> m2DBatch;
		public:
			Scene(const SceneInfo& info);
			virtual ~Scene() = default;
		public:
			void SetSceneInfo(const SceneInfo& info);
			const SceneInfo& GetSceneInfo() const;

			void PushSceneObject(SceneObject* object);
			void PushLight(Light* light);
		private:
			void InitRenderer(ERendererPath path);
			void InitTexture(const JVector2& size, const Color& clearColor);
		};



		// 
		class SceneObject : public GObject
		{
		private:
			JMatrix			 mWorldMatrix;
		public:
			virtual ~SceneObject() = default;


		public:
			void SetWorldMatrix(const JMatrix& m);
			const JMatrix& GetWorldMatrix() const;

		public:
			virtual bool IsValid() const = 0;


		};


		class Render2DObject : public SceneObject
		{
		private:
			Color  mColor = Color::White();
			SharedPtr<ITexture> mTexture = nullptr;
		public:
			virtual ~Render2DObject() = default;
		public:
			void SetColor(const Color& color);
			void SetTexture(SharedPtr<ITexture> tex);


			const Color& GetColor() const;
			SharedPtr<ITexture> GetTexture() const;

		public:
			virtual bool IsValid() const override { return true; }
		};


		class Render3DObject : public SceneObject
		{
		public:
			virtual ~Render3DObject() = default;
		};




		class StaticRenderObject : public Render2DObject
		{
		private:
			SharedPtr<IMesh> mMesh;
			List<SharedPtr<IMaterial>> mMaterialList;
		public:
			virtual ~StaticRenderObject() = default;

		public:
			void SetMesh(SharedPtr<IMesh> mesh);
			void SetMaterialList(const List<SharedPtr<IMaterial>> materialList);


			SharedPtr<IMesh> GetMesh() const;
			const List<SharedPtr<IMaterial>> GetMaterialList() const;
		public:
			virtual bool IsValid() const override;
		};



		/// Light ///
		//
		class Light : public SceneObject
		{
		protected:
			void PushData(List<jbyte>& btData, void* data, u64 size) {
				u64 offset = btData.size();

				btData.resize(offset + size);
				memcpy(&btData[offset], data, size);
			}
		public:
			virtual ~Light() = default;
		public:
			virtual void PushBtData(List<jbyte>& btData) = 0;
			virtual u64 GetBtSize() const = 0;
		};


		//
		class PointLight : public Light
		{
		private:
			JVector3 mColor;
			JVector3 mPosition;
			f32 mIntensity = 1.0f;
			f32 mRange = 0.0f;
			f32 mAtt0 = 0.0f;
			f32 mAtt1 = 0.0f;
			f32 mAtt2 = 0.0f;
		public:
			virtual ~PointLight() = default;
		public:
			void SetColor(const Color& color);
			void SetPosition(const JVector3& position);
			void SetIntensity(f32 intensity);
			void SetRange(f32 range);
			void SetAtt0(f32 att0);
			void SetAtt1(f32 att1);
			void SetAtt2(f32 att2);


			Color GetColor(const Color& color);
			const JVector3& GetPosition() const;
			f32 GetIntensity() const;
			f32 GetRange() const;
			f32 GetAtt0() const;
			f32 GetAtt1() const;
			f32 GetAtt2() const;
		public:
			virtual void PushBtData(List<jbyte>& btData)  override;
			virtual u64 GetBtSize() const override {
				return 44;
			}

		};


	}
}