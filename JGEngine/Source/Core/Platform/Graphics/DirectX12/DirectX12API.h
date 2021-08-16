#pragma once
#include "Graphics/GraphicsAPI.h"
#include "Graphics/GraphicsDefine.h"


enum   DXGI_FORMAT;
struct ID3D12Device;
struct IDXGIFactory4;
struct ID3D12Resource;
struct D3D12_DEPTH_STENCIL_DESC;
struct D3D12_RENDER_TARGET_BLEND_DESC;
struct D3D12_RASTERIZER_DESC;
struct D3D12_CPU_DESCRIPTOR_HANDLE;
namespace JG
{
	class ITexture;
	class  IFrameBuffer;
	class RootSignature;
	class  CommandQueue;
	class  DescriptorAllocation;
	class GraphicsCommandList;
	class ComputeCommandList;
	class CopyCommandList;
	class GraphicsPipelineState;
	class ComputePipelineState;
	class TextureAssetStock;
	class DirectX12API : public IGraphicsAPI
	{
	public:
		virtual EGraphicsAPI GetAPI()const override;
		virtual u64 GetBufferCount() const override;
		virtual u64 GetBufferIndex() const override;
	public:
		static IDXGIFactory4* GetDXGIFactory();
		static ID3D12Device*  GetD3DDevice();
		static CommandQueue*  GetGraphicsCommandQueue();
		static CommandQueue*  GetComputeCommandQueue();
		static CommandQueue*  GetCopyCommandQueue();
		static u64			  GetFrameBufferCount();
		static u64			  GetFrameBufferIndex();


		static DescriptorAllocation RTVAllocate();
		static DescriptorAllocation DSVAllocate();
		static DescriptorAllocation CSUAllocate();

		static GraphicsCommandList* GetGraphicsCommandList(u64 ID);
		static ComputeCommandList*  GetComputeCommandList(u64 ID);
		static CopyCommandList*     GetCopyCommandList(u64 ID);

		static SharedPtr<GraphicsPipelineState> GetGraphicsPipelineState();
		static SharedPtr<ComputePipelineState>  GetComputePipelineState();
		static SharedPtr<RootSignature>			GetRootSignature();

		static void GetDepthStencilDesc(EDepthStencilStateTemplate _template,  D3D12_DEPTH_STENCIL_DESC* out);
		static void GetBlendDesc(EBlendStateTemplate _template,  D3D12_RENDER_TARGET_BLEND_DESC* out);
		static void GetRasterizerDesc(ERasterizerStateTemplate _template,  D3D12_RASTERIZER_DESC* out);
	protected:
		// Application
		virtual bool Create() override;
		virtual void Destroy() override;
		// API
		virtual void Begin() override;
		virtual void End()   override;
		virtual void Flush() override;
	protected:
		virtual void SetViewports(u64 commandID, const List<Viewport>& viewPorts) override;
		virtual void SetScissorRects(u64 commandID, const List<ScissorRect>& scissorRects) override;
		virtual void ClearRenderTarget(u64 commandID, const List<SharedPtr<ITexture>>& rtTextures, SharedPtr<ITexture> depthTexture) override;
		virtual void SetRenderTarget(u64 commandID, const List<SharedPtr<ITexture>>& rtTextures, SharedPtr<ITexture> depthTexture) override;
		virtual void DrawIndexed(u64 commandID, u32 indexCount, u32 instancedCount = 1, u32 startIndexLocation = 0, u32 startVertexLocation = 0, u32 startInstanceLocation = 0) override;
	protected:
		virtual void SetDepthStencilState(EDepthStencilStateTemplate _template) override;
		virtual void SetBlendState(u32 renderTargetSlot, EBlendStateTemplate _template) override;
		virtual void SetRasterizerState(ERasterizerStateTemplate _template) override;
	protected:
		virtual SharedPtr<IFrameBuffer>   CreateFrameBuffer(const FrameBufferInfo& info) override;
		virtual SharedPtr<IVertexBuffer>  CreateVertexBuffer(const String& name, EBufferLoadMethod method) override;
		virtual SharedPtr<IIndexBuffer>   CreateIndexBuffer(const String& name, EBufferLoadMethod method) override;
		virtual SharedPtr<IComputeBuffer> CreateComputeBuffer(const String& name, u64 btSize) override;
		virtual SharedPtr<IComputer>      CreateComputer(const String& name, SharedPtr<IShader> shader) override;
		virtual SharedPtr<IShader>        CreateShader(const String& name, const String& sourceCode, EShaderFlags flags, const List<SharedPtr<IShaderScript>>& scriptList) override;
		virtual SharedPtr<IMaterial>	  CreateMaterial(const String& name) override;
		virtual SharedPtr<IMaterial>	  CreateMaterial(const String& name, SharedPtr<IShader> shader) override;
		virtual SharedPtr<IMesh>		  CreateMesh(const String& name) override;
		virtual SharedPtr<ISubMesh>       CreateSubMesh(const String& name) override;
		virtual SharedPtr<ITexture>       CreateTexture(const String& name) override;
		virtual SharedPtr<ITexture>       CreateTexture(const String& name, const TextureInfo& info) override;
		virtual SharedPtr<ITexture>       CreateTexture(const TextureAssetStock& stock) override;
	};
}
 