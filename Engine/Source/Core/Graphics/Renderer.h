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
		u64 CurrentBufferIndex   = 0;
		u64 CompeleteBufferIndex = 0;

		Color ClearColor;
		bool IsHDR;
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






		// Renderer Param
		Dictionary<Type, Dictionary<String, List<jbyte>>> mLocalParamDic;
		Dictionary<Type, Dictionary<String, std::pair<SharedPtr<ITexture>, bool>>> mLocalParamTexDic;

		Dictionary<String, List<jbyte>>		    mGlobalParamDic;
		Dictionary<String, std::pair<SharedPtr<ITexture>, bool>> mGlobalParamTexDic;
		std::mutex mRenderParamMutex;
	public:
		Renderer() = default;
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
			preProcess->Awake(this);
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
			postProcess->Awake(this);
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





		bool RegisterGlobalRenderParamTex(const String& name,  SharedPtr<ITexture> initTexture = nullptr, bool isPrivate = false);
		bool SetGlobalRenderParamTex(const String& name, SharedPtr<ITexture> tex);
		SharedPtr<ITexture> GetGlobalRenderParamTex(const String& name);



		bool RegisterGlobalRenderParam(const String& name, u64 dataSize);
		template<class T>
		bool SetGlobalRenderParam(const String& name, const T& data)
		{
			if (mGlobalParamDic.find(name) == mGlobalParamDic.end())
			{
				return false;
			}

			auto& btData = mGlobalParamDic[name];
			u64 dataSize = btData.size();
			if (dataSize != sizeof(T))
			{
				return false;
			}

			std::lock_guard<std::mutex> lock(mRenderParamMutex);
			memcpy(btData.data(), (const void*)& data, dataSize);

			return true;
		}
		template<class T>
		bool GetGlobalRenderParam(const String& name, T* out_data)
		{
			if (out_data == nullptr)
			{
				return false;
			}

			if (mGlobalParamDic.find(name) == mGlobalParamDic.end())
			{
				return false;
			}
			auto& btData = mGlobalParamDic[name];
			u64 dataSize = btData.size();
			if (dataSize != sizeof(T))
			{
				return false;
			}

			std::lock_guard<std::mutex> lock(mRenderParamMutex);
			memcpy((void*)out_data, btData.data(), dataSize);
			return true;
		}






		bool RegisterLocalRenderParamTex(const Type& type, const String& name, SharedPtr<ITexture> initTexture = nullptr, bool isPrivate = false);
		bool SetLocalRenderParamTex(const Type& type, const String& name, SharedPtr<ITexture> tex);
		SharedPtr<ITexture> GetLocalRenderParamTex(const Type& type, const String& name);


		bool RegisterLocalRenderParam(const Type& type, const String& name, u64 dataSize);

		template<class DataType>
		bool SetLocalRenderParam(const Type& type, const String& name, const DataType& data)
		{
			if (mLocalParamDic.find(type) == mLocalParamDic.end())
			{
				return false;
			}

			auto& renderParamDic = mLocalParamDic[type];
			if (renderParamDic.find(name) == renderParamDic.end())
			{
				return false;
			}
			List<jbyte>& btData = renderParamDic[name];

			u64 dataSize = sizeof(DataType);
			if (dataSize != btData.size())
			{
				return false;
			}

			std::lock_guard<std::mutex> lock(mRenderParamMutex);
			memcpy((void*)btData.data(), (const void*) & data, dataSize);

			return true;
		}

		template<class DataType>
		bool GetLocalRenderParam(const Type& type, const String& name, DataType* out_data)
		{
			if (out_data == nullptr)
			{
				return false;
			}

			if (mLocalParamDic.find(type) == mLocalParamDic.end())
			{
				return false;
			}

			auto& renderParamDic = mLocalParamDic[type];
			if (renderParamDic.find(name) == renderParamDic.end())
			{
				return false;
			}
			List<jbyte>& btData = renderParamDic[name];
			u64 dataSize = sizeof(DataType);
			if (dataSize != btData.size())
			{
				return false;
			}

			std::lock_guard<std::mutex> lock(mRenderParamMutex);
			memcpy((void*)out_data, btData.data(), dataSize);

			return true;
		}


	protected:
		virtual void ReadyImpl(IGraphicsAPI* api, Graphics::RenderPassData* renderPassData, const RenderInfo& info) = 0;
		virtual void RenderImpl(IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result) = 0;
		virtual void CompeleteImpl(IGraphicsAPI* api, const RenderInfo& info, SharedPtr<RenderResult> result) = 0;
		virtual int  ArrangeObject(const ObjectInfo& info) = 0;
	};
}

