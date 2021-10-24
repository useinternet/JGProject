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
	}
	class JGGraphics : public GlobalSingleton<JGGraphics>
	{

		UniquePtr<IGraphicsAPI> mGraphcisAPI;



	public:
		JGGraphics();
		~JGGraphics();


		Graphics::Scene* CreateScene(const String& name);
		
		IGraphicsAPI* GetGraphicsAPI() const;
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
		public:
			virtual ~GObject() = default;
		public:

			void SetName(const String& name);
			void SetLayer(u64 layer);


			const String& GetName();
			u64 GetLayer();
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
		};


		class Render3DObject : public SceneObject
		{
		public:
			virtual ~Render3DObject() = default;
		};




		class StaticRenderObject : public Render2DObject
		{

		};



		/// Light ///
		//
		class Light : public SceneObject
		{

		};


		//
		class PointLight : public Light
		{

		};


	}
}