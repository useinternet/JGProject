#pragma once
#include "JGCore.h"
#include "GraphicsDefine.h"
#include "RenderParam.h"
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
	class IGraphicsContext;
	class IComputeContext;
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
	class IStructuredBuffer;
	class IByteAddressBuffer;

	class RenderStatistics
	{
	public:
		i32 TotalObjectCount   = 0;
		i32 VisibleObjectCount = 0;
		i32 CullingObjectCount = 0;
	};

	enum class ERenderDebugMode
	{
		None,
		Visible_ActiveCluster,
	};
	class RenderDebugger
	{
	public:
		ERenderDebugMode Mode;
	};


	class RenderInfo
	{
	public:
		JVector2 Resolution;
		JMatrix  ViewProjMatrix;
		JMatrix  ViewMatrix;
		JMatrix  ProjMatrix;
		JVector3 EyePosition;
		f32 FarZ;
		f32 NearZ;
		u64 CurrentBufferIndex = 0;

		Color ClearColor;
	};

	class RenderResult
	{
	public:
		SharedPtr<ITexture> SceneTexture;
	};

	class Renderer 
	{
	public:
		static RenderStatistics Statistics;
		static RenderDebugger   Debugger;
		enum
		{
			RootParam_PointLight,
			RootParam_PassCB,
			RootParam_ObjectCB,
			RootParam_MaterialCB,
			RootParam_Texture2D,
			RootParam_TextureCube,
			RootParam_LightGrid,
			RootParam_VisibleLightIndicies,
		};
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

		SharedPtr<IGraphicsContext> mGraphicsContext;
		SharedPtr<IComputeContext>  mComputeContext;
		UniquePtr<RenderParamManager> mRenderParamManager;
	public:
		Renderer();
		virtual ~Renderer() = default;
	public:
		bool Begin(const RenderInfo& info, List<SharedPtr<Graphics::Light>> lightList, List<SharedPtr<RenderBatch>> batchList);
		void DrawCall(const JMatrix& worldMatrix, SharedPtr<IMesh> mesh, List<SharedPtr<IMaterial>> materialList);
		SharedPtr<RenderResult> End();
		virtual ERendererPath GetRendererPath() const = 0;
	protected:
		bool BeginBatch(const RenderInfo& info, List<SharedPtr<RenderBatch>> batchList);
		void EndBatch();
	public:
		RenderParamManager* GetRenderParamManager() const;
		const RenderInfo&   GetRenderInfo() const;
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

			IRenderProcess::AwakeData awakeData;
			awakeData.pRenderer = this;
			preProcess->Awake(awakeData);
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

			IRenderProcess::AwakeData awakeData;
			awakeData.pRenderer = this;
			postProcess->Awake(awakeData);
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
		virtual void RenderImpl(IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result) = 0;
		virtual void CompeleteImpl(IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result) = 0;
		virtual int  ArrangeObject(const ObjectInfo& info) = 0;
	};
}

