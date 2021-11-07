#pragma once
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
		class Light;
		class PointLight;
		enum class ELightType;
	}
}

namespace JG
{
	class IGraphicsAPI;
	class IRenderContext;
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
	//class IRenderBatch;



	class RenderInfo
	{
	public:
		SharedPtr<ITexture> TargetTexture;
		SharedPtr<ITexture> TargetDepthTexture;
		JVector2 Resolutoin;
		JMatrix  ViewProj;
		JVector3 EyePosition;
		u64 CurrentBufferIndex = 0;
		u64 CommandID = 0;
	};

	// RenderProcess 라는 클래스 추가
	class Renderer 
	{
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
		using DrawFunc      = std::function<void(int, const List<ObjectInfo>&)>;
		using ReadyDrawFunc = std::function<void(IGraphicsAPI*, const RenderInfo& info)>;
	private:
		List<SharedPtr<RenderBatch>> mBatchList;
		Dictionary<Graphics::ELightType, LightInfo>   mLightInfos;
		SortedDictionary<int, List<ObjectInfo>> mObjectInfoListDic;
		List<DrawFunc> mDrawFuncList;
		List<ReadyDrawFunc> mReadyDrawFuncList;

		RenderInfo mCurrentRenderInfo;
	public:
		Renderer() = default;
		virtual ~Renderer() = default;
	public:
		bool Begin(const RenderInfo& info, List<SharedPtr<Graphics::Light>> lightList, List<SharedPtr<RenderBatch>> batchList);
		void DrawCall(const JMatrix& worldMatrix, SharedPtr<IMesh> mesh, List<SharedPtr<IMaterial>> materialList);
		void End();
		u64 GetCommandID() const;
		virtual ERendererPath GetRendererPath() const = 0;
	protected:
		bool BeginBatch(const RenderInfo& info, List<SharedPtr<RenderBatch>> batchList);
		void EndBatch();
	protected:
		const RenderInfo& GetRenderInfo() const;
		const Dictionary<Graphics::ELightType, LightInfo>& GetLightInfos() const;
		void AddDrawFunc(const ReadyDrawFunc& readyFunc, const DrawFunc& drawFunc);
	protected:
		virtual int ArrangeObject(const ObjectInfo& info) = 0;
	};
}

