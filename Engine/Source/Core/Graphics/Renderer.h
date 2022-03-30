#pragma once
#include "JGCore.h"
#include "GraphicsDefine.h"
#include "RenderParam.h"
#include "Develop/RenderStatistics.h"
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
		enum class ESceneObjectFlags;
	}
}

namespace JG
{
	class IGraphicsAPI;
	class IGraphicsContext;
	class IComputeContext;
	class ICopyContext;
	class InputLayout;
	class IShader;
	class ITexture;
	class IMaterial;
	class IRootSignature;
	class IMesh;
	class Application;
	class Camera;
	class IVertexBuffer;
	class IIndexBuffer;
	class RenderBatch;
	class Render2DBatch;
	class FowardRenderer;
	class DeferredRenderer;
	class IRenderProcess;
	class IStructuredBuffer;
	class IByteAddressBuffer;
	class RenderStatistics;
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
		enum class ERootParam : u32
		{
			PointLight,
			PassCB,
			ObjectCB,
			MaterialCB,
			Texture2D,
			TextureCube,
			LightGrid,
			VisibleLightIndicies,
		};


		enum class EComputeRootParam : u32
		{
			UAV,
			SRV,
			CB0
		};
	protected:

		struct LightInfo
		{
			i32 Count = 0;
			u64 Size  = 0;
			List<jbyte> Data;
		};
		struct ObjectInfo
		{
			JMatrix WorldMatrix;
			SharedPtr<IMesh> Mesh;
			List<SharedPtr<IMaterial>> MaterialList;
			Graphics::ESceneObjectFlags Flags;
		};
		struct PaperInfo
		{
			JMatrix WorldMatrix;
			Color  Color = Color::White();
			SharedPtr<ITexture> Texture = nullptr;
		};
	private:
		// Batch , Process
		List<SharedPtr<RenderBatch>> mBatchList;
		Render2DBatch*     mRender2DBatch;



		Dictionary<Type, IRenderProcess*> mProcessPool;
		List<SharedPtr<IRenderProcess>> mPreProcessList;
		List<SharedPtr<IRenderProcess>> mPostProcessList;



		// Infos
		Dictionary<Graphics::ELightType, LightInfo>   mLightInfos;
		SortedDictionary<int ,List<ObjectInfo>> mObjectInfoListDic;
		List<PaperInfo> mPaperInfoList;


		// RenderInfo
		RenderInfo mRenderInfo;
		SharedPtr<RenderResult> mRenderResult;

		// Context, Managers
		SharedPtr<IGraphicsContext> mGraphicsContext;
		SharedPtr<IComputeContext>  mComputeContext;
		SharedPtr<ICopyContext>		mCopyContext;
		UniquePtr<RenderParamManager> mRenderParamManager;

		SharedPtr<IRootSignature> mGraphicsRootSignature;
		SharedPtr<IRootSignature> mComputeRootSignature;




		u64		  mFrameCount = 0;
		const u64 mResultDelayFrameCount = 240;
		SharedPtr<Graphics::RenderPassData> mPassData;
		SharedPtr<IStructuredBuffer> mLightGrid;
		SharedPtr<IStructuredBuffer> mVisibleLightIndicies;
	public:
		Renderer();
		virtual ~Renderer() = default;
	public:
		bool Begin(const RenderInfo& info, List<SharedPtr<Graphics::Light>> lightList);
		void DrawCall(SharedPtr<Graphics::SceneObject> sceneObject);
		SharedPtr<RenderResult> End();
		virtual ERendererPath GetRendererPath() const = 0;

		void SetLightGrid(SharedPtr<IStructuredBuffer> sb);
		void SetVisibleLightIndicies(SharedPtr<IStructuredBuffer> sb);

		SharedPtr<IStructuredBuffer> GetLightGrid() const;
		SharedPtr<IStructuredBuffer> GetVisibleLightIndicies() const;
	protected:
		template<class BatchType>
		BatchType* CreateBatch()
		{
			SharedPtr<BatchType> batch = CreateSharedPtr<BatchType>();
			batch->ConnectRenderer(this);

			mBatchList.push_back(batch);
			return batch.get();
		}
		bool BeginBatch(const RenderInfo& info);
		void EndBatch();
	public:
		SharedPtr<IGraphicsContext> GetGraphicsContext() const;
		SharedPtr<IComputeContext>  GetComputeContext() const;
		SharedPtr<ICopyContext>		GetCopyContext() const;

		SharedPtr<IRootSignature> GetGraphicsRootSignature() const;
		SharedPtr<IRootSignature> GetComputeRootSignature() const;
	public:
		RenderParamManager* GetRenderParamManager() const;
		const RenderInfo&   GetRenderInfo() const;
		RenderResult* GetRenderResult() const;
		const Graphics::RenderPassData& GetPassData() const;
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
	private:
		void InitRootSignature();
	protected:
		virtual void ReadyImpl(Graphics::RenderPassData* renderPassData) = 0;
		virtual void RenderImpl(SharedPtr<RenderResult> result) = 0;
		virtual void CompeleteImpl(SharedPtr<RenderResult> result) = 0;
		virtual int  ArrangeObject(const ObjectInfo& info) = 0;
	};
}

