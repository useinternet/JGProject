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
		class SceneInfo;
		class GObject;
		class SceneObject;
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
		JGGraphics(const String& shaderPath);
		~JGGraphics();


		Graphics::Scene*			  CreateScene(const String& name, const Graphics::SceneInfo& info);


		void DestroyObject(Graphics::GObject* gobject);
		IGraphicsAPI* GetGraphicsAPI() const;
	private:
		template<class T, class ...Args>
		T* CreateGObject(const String& name, Args&& ... args)
		{
			auto gobj = CreateUniquePtr<T>(std::forward<Args>(args)...);
			gobj->SetName(name);
			
			auto result = gobj.get();
			mObjectPool[gobj.get()] = std::move(gobj);
			return result;
		}
		void RemoveObject(Graphics::GObject* gobj)
		{
			mObjectPool.erase(gobj);
		}
		void LoadShader();
	private:
		class RemoveObjectData : public IJGObject
		{
			JGCLASS
		public:
			Graphics::GObject* GObject = nullptr;
			RemoveObjectData(Graphics::GObject* obj) : GObject(obj) {}
		};
	};
}

namespace JG
{
	namespace Graphics
	{
		enum class ELightType
		{
			PointLight,
		};
		enum class ESceneObjectType
		{
			Static,
			Paper,
			Debug,
		};


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

		class SceneResultInfo
		{
		public:
			SharedPtr<ITexture> Texture;
			SharedPtr<ITexture> DepthTexture;
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
			static Queue<u64> sm_CommandIDQueue;


		private:
			SceneInfo mSceneInfo;

			List<SharedPtr<ITexture>> mTargetTextures;
			List<SharedPtr<ITexture>> mTargetDepthTextures;


			Queue<SharedPtr<SceneObject>> mSceneObjectQueue;
			List<SharedPtr<Light>>        mLightList;


			u64 mCurrentIndex = 0;
			u64 mCommandID    = 0;

			SharedPtr<Renderer>       mRenderer;
			SharedPtr<Render2DBatch>  m2DBatch;
			SharedPtr<ScheduleHandle> mRenderScheduleHandle;
		public:
			Scene(const SceneInfo& info);
			virtual ~Scene();
		public:
			void SetSceneInfo(const SceneInfo& info);
			const SceneInfo& GetSceneInfo() const;

			template<class SceneObjectClass>
			void PushSceneObject(const SceneObjectClass& sceneObject)
			{
				auto pSceneObject = CreateSharedPtr<SceneObjectClass>();
				*pSceneObject = sceneObject;

				mSceneObjectQueue.push(pSceneObject);
			}

			template<class LightClass>
			void PushLight(const LightClass& l)
			{
				auto pLight = CreateSharedPtr<LightClass>();
				*pLight = sceneObject;

				mLightList.push_back(pLight);
			}
		public:
			void Rendering();
			SharedPtr<SceneResultInfo> FetchResultFinish();


		private:
			void InitRenderer(ERendererPath path);
			void InitTexture(const JVector2& size, const Color& clearColor);
		};


		// SceneObject //
		class SceneObject
		{
		public:
			JMatrix	WorldMatrix;
			u64     Layer = 0;
		public:
			virtual ESceneObjectType GetSceneObjectType() const = 0;
			virtual bool IsValid() const = 0;
		};


		class PaperObject : public SceneObject
		{
		private:
			Color  Color = Color::White();
			SharedPtr<ITexture> Texture = nullptr;
		public:
			virtual ESceneObjectType GetSceneObjectType() const override { return ESceneObjectType::Paper; }
			virtual bool IsValid() const override { return true; }
		};


		class StaticRenderObject : public SceneObject
		{
		private:
			SharedPtr<IMesh> Mesh;
			List<SharedPtr<IMaterial>> MaterialList;
		public:
			virtual ESceneObjectType GetSceneObjectType() const override { return ESceneObjectType::Paper; }
			virtual bool IsValid() const override { 
				bool result = true;

				if (Mesh == nullptr || Mesh->IsValid() == false)
				{
					result = false;
				}
				if (MaterialList.empty())
				{
					result = false;
				}
				else
				{
					for (auto& m : MaterialList)
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
		};
	


		/// Light ///
		class Light 
		{
		protected:
			void PushData(List<jbyte>& btData, void* data, u64 size) {
				u64 offset = btData.size();

				btData.resize(offset + size);
				memcpy(&btData[offset], data, size);
			}
		public:
			virtual ELightType GetLightType() const = 0;
			virtual void PushBtData(List<jbyte>& btData) = 0;
			virtual u64  GetBtSize() const = 0;
		};


		//
		class PointLight : public Light
		{
		public:
			JVector3 Color;
			JVector3 Position;
			f32 Intensity = 1.0f;
			f32 Range = 0.0f;
			f32 Att0 = 0.0f;
			f32 Att1 = 0.0f;
			f32 Att2 = 0.0f;
		public:
			virtual ELightType GetLightType() const override { return ELightType::PointLight; }
			virtual void PushBtData(List<jbyte>& btData)  override
			{
				PushData(btData, &Position, sizeof(JVector3));
				PushData(btData, &Range, sizeof(float));
				PushData(btData, &Color, sizeof(JVector3));
				PushData(btData, &Intensity, sizeof(float));
				PushData(btData, &Att0, sizeof(float));
				PushData(btData, &Att1, sizeof(float));
				PushData(btData, &Att2, sizeof(float));
			}
			virtual u64 GetBtSize() const override {
				return 44;
			}

		};


	}
}