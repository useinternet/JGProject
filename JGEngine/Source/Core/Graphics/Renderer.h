#pragma once
#include "JGCore.h"
#include "GraphicsDefine.h"
namespace JG
{
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
	class IRenderBatch;



	struct RenderInfo
	{
		SharedPtr<ITexture> TargetTexture;
		SharedPtr<ITexture> TargetDepthTexture;
		JVector2 Resolutoin;
		JMatrix  ViewProj;
		u64 CurrentBufferIndex = 0;
	};


	class IRenderer : public GraphicsCommandable
	{
	private:
		List<SharedPtr<IRenderBatch>> mBatchList;
	public:
		IRenderer() = default;
		virtual ~IRenderer() = default;
	public:
		virtual bool Begin(const RenderInfo& info, List<SharedPtr<IRenderBatch>> batchList) = 0;
		virtual void DrawCall(const JMatrix& worldMatrix, SharedPtr<IMesh> mesh, List<SharedPtr<IMaterial>> materialList) = 0;
		virtual void End() = 0;

		virtual ERendererPath GetRendererPath() const = 0;
	protected:
		bool BeginBatch(const RenderInfo& info, List<SharedPtr<IRenderBatch>> batchList);
		void EndBatch();
	};

	class FowardRenderer : public IRenderer
	{
	private:
		bool mIsRun = false;
		List<SharedPtr<ITexture>> mRenderTarges;
		RenderInfo mCurrentRenderInfo;
	public:
		FowardRenderer() = default;
		virtual ~FowardRenderer() = default;
	public:
		virtual bool Begin(const RenderInfo& info, List<SharedPtr<IRenderBatch>> batchList) override;
		virtual void DrawCall(const JMatrix& worldMatrix, SharedPtr<IMesh> mesh, List<SharedPtr<IMaterial>> materialList) override;
		virtual void End() override;
		virtual ERendererPath GetRendererPath() const override { return ERendererPath::Foward; }
	};

	class IRenderBatch : public GraphicsCommandable
	{
		IRenderer* mConnectedRenderer = nullptr;
	public:
		IRenderBatch() = default;
		virtual ~IRenderBatch() = default;
	public:
		void ConnectRenderer(IRenderer* renderer) {
			mConnectedRenderer = renderer;
		}
		IRenderer* GetConnectedRenderer() const {
			return mConnectedRenderer;
		}

	protected:
		friend IRenderer;
		virtual bool Begin(const RenderInfo& info) = 0;
		virtual void End() = 0;
	};


	class Render2DBatch : public IRenderBatch
	{
	public:
		struct QuadVertex
		{
			JVector3 Pos;
			JVector2 Tex;
			Color _Color = Color::White();
			i32 TextureIndex = 0;
			QuadVertex() = default;
			QuadVertex(const JVector3& pos, const JVector2& tex) : Pos(pos), Tex(tex) {}
			QuadVertex(const JVector3& pos, const JVector2& tex, const Color& color, i32 textureIndex)
				: Pos(pos), Tex(tex), _Color(color), TextureIndex(textureIndex) {}
		};

		struct FrameResource
		{
			SharedPtr<IMesh>    QuadMesh;
			SharedPtr<IVertexBuffer> QuadVBuffer;
			SharedPtr<IIndexBuffer> QuadIBuffer;
			SharedPtr<IMaterial> Standard2DMaterial;
		};
	private:
		static const u32 MaxQuadCount    = 1200;
		static const u32 MaxVertexCount  = MaxQuadCount * 4;
		static const u32 MaxIndexCount   = MaxQuadCount * 6;
		static const u32 MaxTextureCount = 64;

		static const u32 QuadVertexCount = 4;
		static const u32 QuadIndexCount  = 6;
	private:
		JVector3 mStandardQuadPosition[4];
		JVector2 mStandardQuadTexcoord[4];
		List<FrameResource> mFrameResources;
		List<QuadVertex> mVertices;
		List<u32>        mIndices;
		List<SharedPtr<ITexture>> mTextureArray;
		SharedPtr<ITexture> mWhiteTexture;

		u64 mQuadCount = 0;
		u64 mTextureCount = 0;

		FrameResource* mCurrFrameResource = nullptr;
		List<SharedPtr<ITexture>> mRenderTarges;
	public:
		Render2DBatch();
		virtual ~Render2DBatch();
	private:
		virtual bool Begin(const RenderInfo& info) override;
		virtual void End() override;
	public:
		void DrawCall(const JMatrix& transform, SharedPtr<ITexture> texture, const Color& color);
		void DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, SharedPtr<ITexture> texture, const Color& color);
		void DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, const Color& color);
		void DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, SharedPtr<ITexture> texture);
		void DrawCall(const JVector2& Pos, const JVector2& Size, const Color& color);
		void DrawCall(const JVector2& Pos, const JVector2& Size, SharedPtr<ITexture> texture);
	private:
		void StartBatch();
		void NextBatch();
	};
}

