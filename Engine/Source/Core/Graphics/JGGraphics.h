#pragma once
#include "JGCore.h"
#include "GraphicsDefine.h"
#include "GraphicsAPI.h"
#include "Resource.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Renderer.h"
#include "SceneObject.h"
#include "SceneLight.h"
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



	class JGGraphics : public IGlobalSingleton<JGGraphics>
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
		u64			  GetBufferIndex() const;
		bool          IsSupportedRayTracing() const;
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
		void LoadRayTracingPipeline();
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

		class SceneInfo
		{
		public:
			ERendererPath RenderPath = ERendererPath::Deferred;
			JVector2 Resolution;
			JVector3 EyePos;
			JMatrix  ViewMatrix     = JMatrix::Identity();
			JMatrix  ProjMatrix     = JMatrix::Identity();
			JMatrix  ViewProjMatrix = JMatrix::Identity();
			f32 FarZ  = 0.0f;
			f32 NearZ = 0.0f;
			Color  ClearColor;
			Color  AmbientColor = Color(0.25f, 0.25f, 0.25f, 1.0f);
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
			bool IsLock() const;
		};

		class Scene : public GObject
		{
			using PostRenderingEvent = std::function<void(SharedPtr<SceneResultInfo>)>;
		private:
			SceneInfo mSceneInfo;
			Queue<SharedPtr<SceneObject>> mSceneObjectQueue;
			List<SharedPtr<Light>>        mLightList;

			SceneInfo mPendingSceneInfo;
			Queue<SharedPtr<SceneObject>> mPendingSceneObjectQueue;
			List<SharedPtr<Light>>	      mPendingLightList;


			SharedPtr<SceneResultInfo>	  mSceneResult;
			SharedPtr<Renderer>       mRenderer;

			// PushRenderObject
			SharedPtr<ScheduleHandle> mSetSceneInfoSH;
			SharedPtr<ScheduleHandle> mRenderSH;
			SharedPtr<ScheduleHandle> mRenderFetchResultSH;
			SharedPtr<ScheduleHandle> mRenderInternalSH;

			bool mIsRendering = false;

			SortedDictionary<i32, Queue<PostRenderingEvent>> mPostRenderingEventQueue;
		public:
			Scene(const SceneInfo& info);
			virtual ~Scene();
		public:
			void  SetSceneInfo(const SceneInfo& info);
			const SceneInfo& GetSceneInfo() const;

			Renderer* GetRenderer() const;

			bool PushSceneObject(SharedPtr<SceneObject> sceneObject);
			bool PushLight(SharedPtr<Light> l);
			void PushPostRenderingEvent(i32 priority, const PostRenderingEvent& _e);
		public:
			void Reset();
			void Rendering();
			bool IsEnableRendering() const;
			SharedPtr<SceneResultInfo> FetchResult();
		private:
			EScheduleResult RenderScene();
			EScheduleResult RenderFinish();


			void InitRenderer(ERendererPath path);

			
		};

		class RenderPassData
		{
		public:
			JMatrix ProjMatrix;
			JMatrix ViewMatrix;
			JMatrix ViewProjMatrix;
			JMatrix InvViewMatrix;
			JMatrix InvProjMatrix;
			JMatrix InvViewProjMatrix;
			JVector2 Resolution;
			f32 NearZ = 0.0f;
			f32 FarZ  = 0.0f;
			JVector3 EyePosition;
			u32 PointLightCount = 0;
			JVector2 ClusterSize;
			f32 ClusterScale = 0.0f;
			f32 ClusterBias = 0.0f;
			JVector3Uint NumClusterSlice;
			u32 FrameCount = 0;
			u32 DirectionalLightCount = 0;
			u32 SpotLightCount = 0;
		};

		class LightGrid
		{
		public:
			i32 PL_Count  = 0;
			i32 PL_Offset = 0;
		};
	}
}