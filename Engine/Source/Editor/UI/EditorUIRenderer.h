#pragma once
#include "JGCore.h"


namespace JG
{
	// Editor UI 용으로 제작
	// SceneView 에 들어가는 아이콘을 그리는 렌더러
	class IMesh;
	class ITexture;
	class IVertexBuffer;
	class IIndexBuffer;
	class IMaterial;
	class EditorUIRenderer
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
			SharedPtr<IMesh>         QuadMesh;
			SharedPtr<IVertexBuffer> QuadVBuffer;
			SharedPtr<IIndexBuffer>  QuadIBuffer;
		};
	private:
		static const u32 MaxQuadCount = 256;
		static const u32 MaxVertexCount = MaxQuadCount * 4;
		static const u32 MaxIndexCount  = MaxQuadCount * 6;
		static const u32 MaxTextureCount = 64;

		static const u32 QuadVertexCount = 4;
		static const u32 QuadIndexCount = 6;
	private:
		JVector3 mStandardQuadPosition[4];
		JVector2 mStandardQuadTexcoord[4];
		List<FrameResource> mFrameResources;
		SharedPtr<IMaterial> mEditorUIMaterial;
		List<QuadVertex>    mVertices;
		List<u32>           mIndices;
		List<SharedPtr<ITexture>> mTextureArray;
		SharedPtr<ITexture>       mWhiteTexture;

		u64 mQuadCount = 0;
		u64 mTextureCount = 0;

		FrameResource* mCurrFrameResource = nullptr;
		bool mIsClearWhiteTexture = false;
	public:
		EditorUIRenderer();
		bool Begin(SharedPtr<ITexture> targetTexture);
		void End();
		void DrawCall(const JMatrix& transform, SharedPtr<ITexture> texture, const Color& color);
	private:
		void Init();
		void StartBatch();
		void NextBatch();
	};
}