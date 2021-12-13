#pragma once
#include "JGCore.h"
#include "GraphicsDefine.h"

namespace JG
{
	/* 넣어야할 설정 값
	Renderer2D Shader

	
	*/

	enum class EShaderFlags;
	class ITexture;
	class IVertexBuffer;
	class IIndexBuffer;
	class IGraphicsShader;
	class IComputeShader;
	class IShaderScript;
	class IFrameBuffer;
	class IMaterial;
	class IMesh;
	class IReadWriteBuffer;
	class IReadBackBuffer;
	class IComputer;
	class ISubMesh;
	
	struct TextureInfo;
	struct FrameBufferInfo;

	class TextureAssetStock;

	namespace Graphics
	{
		class RenderPassData;
		class Light;
	}
	class IGraphicsAPI 
	{
	public:
		virtual EGraphicsAPI GetAPI() const = 0;
		virtual u64 GetBufferCount() const = 0;
		virtual u64 GetBufferIndex() const = 0;
	protected:
		friend class Application;
		friend class JGGraphics;
		virtual bool Create() = 0;
		virtual void Destroy() = 0;

	public:
		virtual void BeginFrame() = 0;
		virtual void EndFrame()	 = 0;
		virtual void Flush() = 0;
		//
		virtual void BeginDraw(u64 commandID) = 0;
		virtual void EndDraw(u64 commandID)   = 0;
		virtual void SetRenderPassData(u64 commandID, const Graphics::RenderPassData& passData) = 0;
		virtual void SetLights(u64 commandID, const List<SharedPtr<Graphics::Light>>& lights) = 0;
		virtual void SetTextures(u64 commandID, const List<SharedPtr<ITexture>>& textures) = 0;
		virtual void SetTransform(u64 commandID, const JMatrix* worldmats, u64 instanceCount = 1) = 0;
		virtual void SetViewports(u64 commandID, const List<Viewport>& viewPorts) = 0;
		virtual void SetScissorRects(u64 commandID, const List<ScissorRect>& scissorRects) = 0;
		virtual void ClearRenderTarget(u64 commandID, const List<SharedPtr<ITexture>>& rtTextures, SharedPtr<ITexture> depthTexture) = 0;
		virtual void SetRenderTarget(u64 commandID, const List<SharedPtr<ITexture>>& rtTextures, SharedPtr<ITexture> depthTexture)   = 0;
		virtual void DrawIndexed(u64 commandID, u32 indexCount, u32 instancedCount = 1, u32 startIndexLocation = 0, u32 startVertexLocation = 0, u32 startInstanceLocation = 0) = 0;
		// State

		virtual void SetDepthStencilState(u64 commandID, EDepthStencilStateTemplate _template) = 0;
		virtual void SetBlendState(u64 commandID, u32 renderTargetSlot, EBlendStateTemplate _template) = 0;
		virtual void SetRasterizerState(u64 commandID, ERasterizerStateTemplate _template) = 0;

		// Create Resource
		virtual SharedPtr<IFrameBuffer>   CreateFrameBuffer(const FrameBufferInfo& settings) = 0;
		virtual SharedPtr<IVertexBuffer>  CreateVertexBuffer(const String& name, EBufferLoadMethod method) = 0;
		virtual SharedPtr<IIndexBuffer>   CreateIndexBuffer(const String& name, EBufferLoadMethod method) = 0;
		

		virtual SharedPtr<IReadWriteBuffer> CreateReadWriteBuffer(const String& name, u64 btSize) = 0;
		virtual SharedPtr<IReadBackBuffer>  CreateReadBackBuffer(const String& name) = 0;
		virtual SharedPtr<IReadBackBuffer>  CreateReadBackBuffer(const String& name, SharedPtr<IReadWriteBuffer> readWriteBuffer) = 0;
		virtual SharedPtr<IComputer>      CreateComputer(const String& name, SharedPtr<IComputeShader> shader) = 0;
		virtual SharedPtr<IGraphicsShader> CreateGraphicsShader(const String& sourceCode, EShaderFlags flags, const List<SharedPtr<IShaderScript>>& scriptList) = 0;
		virtual SharedPtr<IComputeShader>  CreateComputeShader(const String& sourceCode) = 0;
		virtual SharedPtr<IMaterial>	  CreateMaterial(const String& name) = 0;
		virtual SharedPtr<IMaterial>	  CreateMaterial(const String& name, SharedPtr<IGraphicsShader> shader) = 0;
		virtual SharedPtr<IMesh>          CreateMesh(const String& name) = 0;
		virtual SharedPtr<ISubMesh>       CreateSubMesh(const String& name) = 0;
		virtual SharedPtr<ITexture>       CreateTexture(const String& name) = 0;
		virtual SharedPtr<ITexture>       CreateTexture(const String& name, const TextureInfo& info) = 0;
		virtual SharedPtr<ITexture>       CreateTexture(const TextureAssetStock& stock) = 0;
		virtual void ClearTexture(u64 commandID, SharedPtr<ITexture> texture);
	public:
		static UniquePtr<IGraphicsAPI> Create(EGraphicsAPI api);
		
	};


}















