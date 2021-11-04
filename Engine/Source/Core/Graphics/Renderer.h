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
	};

	// RenderProcess 라는 클래스 추가
	class Renderer : public GraphicsCommandable
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

	//class FowardRenderer : public Renderer
	//{
	//public:
	//	FowardRenderer();
	//	virtual ~FowardRenderer() = default;
	//public:
	//	virtual ERendererPath GetRendererPath() const override { return ERendererPath::Foward; }
	//	virtual int ArrangeObject(const ObjectInfo& info) override;
	//private:
	//	void Draw(int objectType, const List<ObjectInfo>& objectList);
	//	
	//};

	//class IRenderBatch : public GraphicsCommandable
	//{
	//	Renderer* mConnectedRenderer = nullptr;
	//public:
	//	IRenderBatch() = default;
	//	virtual ~IRenderBatch() = default;
	//public:
	//	void ConnectRenderer(Renderer* renderer) {
	//		mConnectedRenderer = renderer;
	//	}
	//	Renderer* GetConnectedRenderer() const {
	//		return mConnectedRenderer;
	//	}

	//protected:
	//	friend Renderer;
	//	virtual bool Begin(const RenderInfo& info) = 0;
	//	virtual void End() = 0;
	//};


	//class Render2DBatch : public IRenderBatch
	//{
	//public:
	//	struct QuadVertex
	//	{
	//		JVector3 Pos;
	//		JVector2 Tex;
	//		Color _Color = Color::White();
	//		i32 TextureIndex = 0;
	//		QuadVertex() = default;
	//		QuadVertex(const JVector3& pos, const JVector2& tex) : Pos(pos), Tex(tex) {}
	//		QuadVertex(const JVector3& pos, const JVector2& tex, const Color& color, i32 textureIndex)
	//			: Pos(pos), Tex(tex), _Color(color), TextureIndex(textureIndex) {}
	//	};

	//	struct FrameResource
	//	{
	//		SharedPtr<IMesh>    QuadMesh;
	//		SharedPtr<IVertexBuffer> QuadVBuffer;
	//		SharedPtr<IIndexBuffer> QuadIBuffer;
	//		SharedPtr<IMaterial> Standard2DMaterial;
	//	};
	//private:
	//	static const u32 MaxQuadCount    = 1200;
	//	static const u32 MaxVertexCount  = MaxQuadCount * 4;
	//	static const u32 MaxIndexCount   = MaxQuadCount * 6;
	//	static const u32 MaxTextureCount = 64;

	//	static const u32 QuadVertexCount = 4;
	//	static const u32 QuadIndexCount  = 6;
	//private:
	//	JVector3 mStandardQuadPosition[4];
	//	JVector2 mStandardQuadTexcoord[4];
	//	List<FrameResource> mFrameResources;
	//	List<QuadVertex> mVertices;
	//	List<u32>        mIndices;
	//	List<SharedPtr<ITexture>> mTextureArray;
	//	SharedPtr<ITexture> mWhiteTexture;

	//	u64 mQuadCount = 0;
	//	u64 mTextureCount = 0;

	//	FrameResource* mCurrFrameResource = nullptr;
	//	List<SharedPtr<ITexture>> mRenderTarges;
	//public:
	//	Render2DBatch();
	//	virtual ~Render2DBatch();
	//private:
	//	virtual bool Begin(const RenderInfo& info) override;
	//	virtual void End() override;
	//public:
	//	void DrawCall(const JMatrix& transform, SharedPtr<ITexture> texture, const Color& color);
	//	void DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, SharedPtr<ITexture> texture, const Color& color);
	//	void DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, const Color& color);
	//	void DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, SharedPtr<ITexture> texture);
	//	void DrawCall(const JVector2& Pos, const JVector2& Size, const Color& color);
	//	void DrawCall(const JVector2& Pos, const JVector2& Size, SharedPtr<ITexture> texture);
	//private:
	//	void StartBatch();
	//	void NextBatch();
	//};
}

