﻿#pragma once
#include "JGCore.h"
#include "GraphicsDefine.h"

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
		class RenderPassData;
		class Light;
		class PointLight;
		enum class ELightType;
	}
}

namespace JG
{
	class IGraphicsAPI;
	class InputLayout;
	class IShader;
	class ITexture;
	class IMaterial;
	class IMesh;
	class Application;
	class Camera;
	class IVertexBuffer;
	class IIndexBuffer;
	class RenderBatch;
	class FowardRenderer;
	class DeferredRenderer;
	class IRenderProcess;

	class RenderStatistics
	{
	public:
		i32 TotalObjectCount   = 0;
		i32 VisibleObjectCount = 0;
		i32 CullingObjectCount = 0;
	};


	class RenderInfo
	{
	public:
		SharedPtr<ITexture> TargetTexture;
		SharedPtr<ITexture> TargetDepthTexture;
		JVector2 Resolutoin;
		JMatrix  ViewProjMatrix;
		JMatrix  ViewMatrix;
		JMatrix  ProjMatrix;
		JVector3 EyePosition;
		f32 FarZ;
		f32 NearZ;
		u64 CurrentBufferIndex = 0;
	};




	class Renderer 
	{
	public:
		static RenderStatistics Statistics;
	protected:
		struct LightInfo
		{
			i32 Count = 0;
			u64 Size = 0;
			List<jbyte> ByteData;
		};
		struct ObjectInfo
		{
			JMatrix WorldMatrix;
			SharedPtr<IMesh> Mesh;
			List<SharedPtr<IMaterial>> MaterialList;
		};

	private:
		List<SharedPtr<RenderBatch>>	mBatchList;
		Dictionary<Type, IRenderProcess*> mProcessPool;
		List<SharedPtr<IRenderProcess>> mPreProcessList;
		List<SharedPtr<IRenderProcess>> mPostProcessList;



		Dictionary<Graphics::ELightType, LightInfo>   mLightInfos;
		SortedDictionary<int ,List<ObjectInfo>> mObjectInfoListDic;

		RenderInfo mCurrentRenderInfo;
	public:
		Renderer() = default;
		virtual ~Renderer() = default;
	public:
		bool Begin(const RenderInfo& info, List<SharedPtr<Graphics::Light>> lightList, List<SharedPtr<RenderBatch>> batchList);
		void DrawCall(const JMatrix& worldMatrix, SharedPtr<IMesh> mesh, List<SharedPtr<IMaterial>> materialList);
		void End();
		virtual ERendererPath GetRendererPath() const = 0;
	protected:
		bool BeginBatch(const RenderInfo& info, List<SharedPtr<RenderBatch>> batchList);
		void EndBatch();
	public:
		const RenderInfo& GetRenderInfo() const;
		const Dictionary<Graphics::ELightType, LightInfo>&       GetLightInfos() const;
		const LightInfo& GetLightInfo(Graphics::ELightType type);
		const SortedDictionary<int, List<Renderer::ObjectInfo>>& GetObjectInfoLists() const;

		void ForEach(const std::function<void(Graphics::ELightType, const LightInfo&)>& action);
		void ForEach(const std::function<void(i32, const List<ObjectInfo>&)>& action);

		template<class T>
		T* AddPreProcess()
		{
			Type type = JGTYPE(T);
			if (mProcessPool.find(type) != mProcessPool.end())
			{
				return static_cast<T*>(mProcessPool[type]);
			}

			auto preProcess = CreateSharedPtr<T>();
			mPreProcessList.push_back(preProcess);
			
			mProcessPool[type] = preProcess.get();

			return preProcess.get();
		}
		template<class T>
		T* AddPostProcess()
		{
			Type type = JGTYPE(T);
			if (mProcessPool.find(type) != mProcessPool.end())
			{
				return static_cast<T*>(mProcessPool[type]);
			}

			auto postProcess = CreateSharedPtr<T>();
			mPostProcessList.push_back(postProcess);

			mProcessPool[type] = postProcess.get();
			return postProcess.get();
		}

		template<class T>
		T* FindProcess()
		{
			Type type = JGTYPE(T);
			if (mProcessPool.find(type) != mProcessPool.end())
			{
				return static_cast<T*>(mProcessPool[type]);
			}
			else
			{
				return nullptr;
			}
		}
	protected:
		virtual void ReadyImpl(IGraphicsAPI* api, Graphics::RenderPassData* renderPassData, const RenderInfo& info) = 0;
		virtual void RenderImpl(IGraphicsAPI* api, const RenderInfo& info) = 0;
		virtual void CompeleteImpl(IGraphicsAPI* api, const RenderInfo& info) = 0;
		virtual int  ArrangeObject(const ObjectInfo& info) = 0;
	};
}

