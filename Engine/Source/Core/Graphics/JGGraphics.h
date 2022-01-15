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
	class Render2DBatch;
	class DebugGeometryDrawer;



	struct JGGraphicsDesc
	{
		EGraphicsAPI GraphicsAPI;
		String GlobalShaderLibPath;
		String ShaderTemplatePath;
		String ShaderScriptPath;
	};
	class JGGraphics : public GlobalSingleton<JGGraphics, JGGraphicsDesc>
	{
		friend class Application;
	private:
		std::mutex mMutex;
		Dictionary<Graphics::GObject*, UniquePtr<Graphics::GObject>> mObjectPool;
		Dictionary<Graphics::GObject*, Graphics::Scene*> mSceneDic;
	

		UniquePtr<IGraphicsAPI> mGraphcisAPI;
		UniquePtr<DebugGeometryDrawer> mDebugGeometryDrawer;
		JGGraphicsDesc mDesc;


	
	public:
		JGGraphics(const JGGraphicsDesc& desc);
		~JGGraphics();

		Graphics::Scene*			  CreateScene(const String& name, const Graphics::SceneInfo& info);



		void DestroyObject(Graphics::GObject* gobject);
		
		void ForEach(const std::function<void(Graphics::Scene*)>& action);

		DebugGeometryDrawer* GetDebugGeometryDrawer() const;
		IGraphicsAPI* GetGraphicsAPI() const;
		u64           GetBufferCount() const;
		const JGGraphicsDesc& GetDesc() const;
		void  Flush();
	private:
		template<class T, class ...Args>
		T* CreateGObject(const String& name, Args&& ... args)
		{
			std::lock_guard<std::mutex> lock(mMutex);
			auto gobj = CreateUniquePtr<T>(std::forward<Args>(args)...);
			gobj->SetName(name);
			
			auto result = gobj.get();
			mObjectPool[gobj.get()] = std::move(gobj);
			return result;
		}
		void RemoveObject(Graphics::GObject* gobj)
		{
			std::lock_guard<std::mutex> lock(mMutex);
			mObjectPool.erase(gobj);
		}
		void Init();
		void Reset();
		void LoadShader();
		void LoadShaderTemplate();
		void LoadShaderScript();
		void LoadComputeShader();
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
			f32 FarZ  = 0.0f;
			f32 NearZ = 0.0f;
			Color    ClearColor;
		};

		class SceneResultInfo
		{
		public:
			SharedPtr<ITexture> Texture;
		};


		class GObject
		{
		private:
			String mName;
			bool   mLock = false;
		public:
			virtual ~GObject() = default;
		public:
			void SetName(const String& name);
			const String& GetName();
		protected:
			void Lock();
			void UnLock();
			bool IsLock();
		};


		//
		class Scene : public GObject
		{
			using PostRenderingEvent = std::function<void(SharedPtr<SceneResultInfo>)>;
		private:
			SceneInfo mSceneInfo;
			SharedPtr<SceneResultInfo>	  mSceneResult;
			Queue<SharedPtr<SceneObject>> mSceneObjectQueue;
			List<SharedPtr<Light>>        mLightList;


			u64 mCurrentIndex = 0;

			SharedPtr<Renderer>       mRenderer;
			SharedPtr<Render2DBatch>  m2DBatch;
			SharedPtr<ScheduleHandle> mRenderScheduleHandle;


			SortedDictionary<i32, Queue<PostRenderingEvent>> mPostRenderingEventQueue;
		public:
			Scene(const SceneInfo& info);
			virtual ~Scene();
		public:
			bool SetSceneInfo(const SceneInfo& info);
			const SceneInfo& GetSceneInfo() const;
			Renderer* GetRenderer() const;

			bool PushSceneObject(SharedPtr<SceneObject> sceneObject);
			bool PushLight(SharedPtr<Light> l);
			void PushPostRenderingEvent(i32 priority, const PostRenderingEvent& _e);

		public:
			void Reset();
			void Rendering();
			SharedPtr<SceneResultInfo> FetchResultFinish();


		private:
			void InitRenderer(ERendererPath path);
		};


		// SceneObject //
		class SceneObject
		{
		public:
			JMatrix	WorldMatrix = JMatrix::Identity();
			u64     Layer = 0;
		public:
			virtual ~SceneObject() = default;
		public:
			virtual ESceneObjectType GetSceneObjectType() const = 0;
			virtual bool IsValid() const = 0;
		};

		class PaperObject : public SceneObject
		{
		public:
			Color  Color = Color::White();
			SharedPtr<ITexture> Texture = nullptr;
		public:
			virtual ~PaperObject() = default;
		public:
			virtual ESceneObjectType GetSceneObjectType() const override { return ESceneObjectType::Paper; }
			virtual bool IsValid() const override { return true; }
		};

		class DebugRenderObject : public SceneObject
		{
		public:
			SharedPtr<IMesh> Mesh;
			List<SharedPtr<IMaterial>> MaterialList;
		public:
			virtual ~DebugRenderObject() = default;
		public:
			virtual ESceneObjectType GetSceneObjectType() const override { return ESceneObjectType::Debug; }
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

		class StaticRenderObject : public SceneObject
		{
		public:
			SharedPtr<IMesh> Mesh;
			List<SharedPtr<IMaterial>> MaterialList;
		public:
			virtual ~StaticRenderObject() = default;
		public:
			virtual ESceneObjectType GetSceneObjectType() const override { return ESceneObjectType::Static; }
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
		public:
			virtual ~Light() = default;
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
			virtual ~PointLight() = default;
		public:
			JVector3 Color;
			JVector3 Position;
			f32 Intensity = 1.0f;
			f32 Range    = 0.0f;
			f32 AttRange = 0.0f;
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
				PushData(btData, &AttRange, sizeof(float));
				PushData(btData, &Intensity, sizeof(float));
				PushData(btData, &Att0, sizeof(float));
				PushData(btData, &Att1, sizeof(float));
				PushData(btData, &Att2, sizeof(float));
			}
			virtual u64 GetBtSize() const override {
				return 48;
			}
		};


		class RenderPassData
		{
		public:
			JMatrix ProjMatrix;
			JMatrix ViewMatrix;
			JMatrix ViewProjMatrix;
			JVector2 Resolution;
			f32 NearZ = 0.0f;
			f32 FarZ  = 0.0f;
			JVector3 EyePosition;
			u32 PointLightCount = 0;
			JVector2 ClusterSize;
			f32 ClusterScale = 0.0f;
			f32 ClusterBias = 0.0f;
			JVector3Uint NumClusterSlice;
			u32 DebugMode = 0;
		};

		class LightGrid
		{
		public:
			i32 PL_Count  = 0;
			i32 PL_Offset = 0;
		};
	}
}